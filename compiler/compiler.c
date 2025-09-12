#include <ctype.h>
#include "../types/typization.h"
#include "asm.h"
#include "../ir/errors.h"



#include "lib.h"

char* allocTemp();

#define USE_BOOTSTRAP 1
#ifndef ENABLE_DYNAMIC_RUNTIME
#define ENABLE_DYNAMIC_RUNTIME 0
#endif

static int gFrameSize = 0;
static char* g_func_epilogue_label = NULL;

int nextFreeAddress = 0x1002;
bool g_is_main = false;

static const int IO_ADDR_OP  = 4104; // operator storage
static const int IO_ADDR_IN0 = 4106; // first numeric input base
static int g_io_in_count = 0;        // how many numeric inputs have been bound

#if ENABLE_DYNAMIC_RUNTIME

static char* call_dyn_binary(const char* fname, const char* lhsAtom, const char* rhsAtom) {
    if (lhsAtom) emit_mov("r0", (char*)lhsAtom); else emit_mov("r0", "0");
    if (rhsAtom) emit_mov("r1", (char*)rhsAtom); else emit_mov("r1", "0");
    emit_call(fname);
    return strdup("r0");
}
#endif

symbolTable* globalSymTable;
extern int irCount;

typedef struct { char* name; int addr; } LocalSym;
static LocalSym gLocalSyms[1024];
static int gLocalSymCount = 0;

static void locals_reset(void) {
    for (int i = 0; i < gLocalSymCount; ++i) free(gLocalSyms[i].name);
    gLocalSymCount = 0;
    gFrameSize = 0;
    // ВАЖНО: сбрасываем аллокатор памяти под переменные на стартовый адрес.
    nextFreeAddress = 0x1002; // 4098
    g_io_in_count = 0;
}


static bool locals_lookup(const char* name, int* outAddr) {
    if (!name) return false;
    for (int i = 0; i < gLocalSymCount; ++i) {
        if (strcmp(gLocalSyms[i].name, name) == 0) {
            if (outAddr) *outAddr = gLocalSyms[i].addr;
            return true;
        }
    }
    return false;
}

static int is_ident_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}
static int looks_like_ident_strict(const char* s) {
    if (!s || !*s) return 0;
    // имя вида [A-Za-z_][A-Za-z0-9_]*
    if (!(isalpha((unsigned char)s[0]) || s[0]=='_')) return 0;
    for (const char* p=s+1; *p; ++p)
        if (!(isalnum((unsigned char)*p) || *p=='_')) return 0;

    // служебки/стримы нам не нужны
    if (!strcmp(s,"cin") || !strcmp(s,"cout") || !strcmp(s,"endl")) return 0;
    if (!strncmp(s,"OP_",3)) return 0;

    return 1;
}

static void locals_insert(const char* name, int addr) {
    if (!name) return;
    if (gLocalSymCount < (int)(sizeof(gLocalSyms)/sizeof(gLocalSyms[0]))) {
        gLocalSyms[gLocalSymCount].name = strdup(name);
        gLocalSyms[gLocalSymCount].addr = addr;
        ++gLocalSymCount;
    }
}

static bool findSymbolAddressByName(symbolTable* table, const char* name, int* outAddr) {
    if (!table || !name) return false;
    for (int i = 0; i < table->count; ++i) {
        if (table->symbols[i].name && strcmp(table->symbols[i].name, name) == 0) {
            if (outAddr) *outAddr = table->symbols[i].address;
            return true;
        }
    }
    return false;
}
static const char* regByAddr(int addr) {
    switch (addr) {
        case -1: return "r0";
        case -2: return "r1";
        case -3: return "r2";
        case -4: return "r3";
        default: return NULL;
    }
}
static void plan_locals_in_tree(struct parseTree* pt) {
    if (!pt) return;

    // 0) если это чистый лист
    if (!pt->left && !pt->right) {
        const char* nm = pt->name ? pt->name : "";
        if (looks_like_ident_strict(nm)) {
            int dummy, addr = 0;

            // если это параметр, у которого уже есть "адрес" = регистр (отрицательные -1..-4), то пропускаем
            if (findSymbolAddressByName(globalSymTable, nm, &addr)) {
                if (regByAddr(addr) != NULL) return; // r0..r3
            }

            if (!locals_lookup(nm, &dummy)) {
                int a = nextFreeAddress; nextFreeAddress += 2;
                locals_insert(nm, a);
                // gFrameSize нам больше не нужен, но можно вести для совместимости:
                gFrameSize += 2;
            }
        }
        return;
    }

    // 1) специализированный случай: OP_PLACE (lvalue-узлы)
    if (!strcmp(pt->name,"OP_PLACE") && pt->left && pt->left->name) {
        const char* nm = pt->left->name;
        if (looks_like_ident_strict(nm)) {
            int dummy, addr=0;
            if (findSymbolAddressByName(globalSymTable, nm, &addr)) {
                if (regByAddr(addr) != NULL) return;
            }
            if (!locals_lookup(nm, &dummy)) {
                int a = nextFreeAddress; nextFreeAddress += 2;
                locals_insert(nm, a);
                gFrameSize += 2;
            }
        }
    }

    // 2) VarDecl — оставляем как было (ничего не ломаем)
    if (!strcmp(pt->name,"VarDecl") || !strcmp(pt->name,"VarDeclToken")) {
        struct parseTree* idNode = pt->left;
        if (idNode && idNode->name && looks_like_ident_strict(idNode->name)) {
            int dummy;
            if (!locals_lookup(idNode->name, &dummy)) {
                int a = nextFreeAddress; nextFreeAddress += 2;
                locals_insert(idNode->name, a);
                gFrameSize += 2;
            }
        }
    }

    // 3) обычный обход
    plan_locals_in_tree(pt->left);
    plan_locals_in_tree(pt->right);
}

static bool isOperation(char* ptName) {
    const char* binaryOps[] = {
        "OP_ADD", "OP_SUB", "OP_MUL", "OP_DIV", "OP_MOD",
        "OP_LSHIFT", "BINOP_RSHIFT", "BINOP_AND", "OP_XOR", "OP_OR",
        "OP_ASSIGN"
    };
    for (size_t i = 0; i < sizeof(binaryOps) / sizeof(binaryOps[0]); ++i) {
        if (strcmp(ptName, binaryOps[i]) == 0) {
            return true;
        }
    }
    return false;
}

int tempCounter = 0;
;


char* allocTemp() {
    char* buf = malloc(10);
    sprintf(buf, "t%d", tempCounter++);
    return buf;
}



static bool isRegName(const char* s) {
    return s && (
        strcmp(s, "r0") == 0 ||
        strcmp(s, "r1") == 0 ||
        strcmp(s, "r2") == 0 ||
        strcmp(s, "r3") == 0
    );
}



static struct parseTree* unwrapExpr(struct parseTree* n) {
    if (!n) return NULL;
    if (strcmp(n->name, "ExpressionToken") == 0 || strcmp(n->name, "Expression") == 0) {
        return n->left ? n->left : n->right;
    }
    return n;
}

// Returns non-zero if s looks like an immediate literal (dec/hex/bin/char)
static int is_immediate_literal(const char* s) {
    if (!s || !*s) return 0;
    // char literal: 'x'
    size_t n = strlen(s);
    if (n >= 3 && s[0] == '\'' && s[n-1] == '\'') return 1;

    // hex: 0x...
    if (n >= 3 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        for (size_t i = 2; i < n; ++i) {
            if (!( (s[i] >= '0' && s[i] <= '9') ||
                   (s[i] >= 'a' && s[i] <= 'f') ||
                   (s[i] >= 'A' && s[i] <= 'F') )) return 0;
        }
        return 1;
    }
    // binary: 0b...
    if (n >= 3 && s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) {
        for (size_t i = 2; i < n; ++i) {
            if (!(s[i] == '0' || s[i] == '1')) return 0;
        }
        return 1;
    }
    // decimal: all digits
    int all_digit = 1;
    for (size_t i = 0; i < n; ++i) {
        if (!(s[i] >= '0' && s[i] <= '9')) { all_digit = 0; break; }
    }
    if (all_digit) return 1;

    return 0;
}

static int parse_char_lit(const char* s, int* out) {
    size_t n = s ? strlen(s) : 0;
    if (n >= 3 && s[0]=='\'' && s[n-1]=='\'') {
        if (n == 3) { *out = (unsigned char)s[1]; return 1; }
        if (n == 4 && s[1]=='\\') {
            char c = s[2];
            int v = (c=='n')?10 : (c=='t')?9 : (c=='r')?13 : (unsigned char)c;
            *out = v; return 1;
        }
    }
    return 0;
}

static int one_char_dq_string_to_code(const char* s, int* out) {
    size_t n = s ? strlen(s) : 0;
    if (n == 3 && s[0]=='"' && s[2]=='"') { *out = (unsigned char)s[1]; return 1; }
    if (n == 4 && s[0]=='"' && s[1]=='\\' && s[3]=='"') {
        char c = s[2];
        int v = (c=='n')?10 : (c=='t')?9 : (c=='r')?13 : (unsigned char)c;
        *out = v; return 1;
    }
    return 0;
}

// compiler.c
static char* resolveLeafAtom(const char* name) {
    if (!name) return strdup("0");

    // 1) символьные литералы: вернуть ИММЕДИАТ (без emit_mov здесь!)
    int ch = 0;
    if (parse_char_lit(name, &ch)) {
        char buf[16]; snprintf(buf, sizeof(buf), "%d", ch);
        return strdup(buf);
    }
    if (one_char_dq_string_to_code(name, &ch)) {
        char buf[16]; snprintf(buf, sizeof(buf), "%d", ch);
        return strdup(buf);
    }

    // 2) чистый литерал: вернуть строку-иммедиат
    if (is_immediate_literal(name)) {
        return strdup(name);
    }

    // 3) регистры/временные значения — вернуть как есть
    if (isRegName(name) || (name[0]=='t' && isdigit((unsigned char)name[1]))) {
        return strdup(name);
    }

    // 4) глобальные символы / регистровые псевдоадреса
    int addr = 0;
    if (findSymbolAddressByName(globalSymTable, name, &addr)) {
        const char* r = regByAddr(addr);
        if (r) {
            return strdup(r); // переменная — это регистр r0..r3
        } else if (addr > 0) {
            // память: реально загрузим в r0 и вернём r0
            char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
            emit_load(strdup(buf), "r0");
            return strdup("r0");
        }
    }

    // 5) локальные (через fp-смещения или абсолют)
    if (locals_lookup(name, &addr)) {
        const char* r = regByAddr(addr);
        if (r) return strdup(r);

        if (addr <= 0) {
            // loadfp [fp+off] → r0
            emit_load_fp(addr, "r0");
            return strdup("r0");
        } else {
            char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
            emit_load(strdup(buf), "r0");
            return strdup("r0");
        }
    }

    // 6) ленивое размещение новой переменной в памяти
    int lazy = nextFreeAddress; nextFreeAddress += 2;
    locals_insert(name, lazy);
    char buf[32]; snprintf(buf, sizeof(buf), "%d", lazy);
    emit_load(strdup(buf), "r0");
    return strdup("r0");
}

static char* extractIdentFromLValue(struct parseTree* node) {
    if (!node) return strdup("BAD_LVALUE");
    if (strcmp(node->name, "OP_PLACE") == 0 && node->left) {
        struct parseTree* id = node->left;
        if (id && !id->left && !id->right) {
            return strdup(id->name);
        }
    }
    if (!node->left && !node->right) {
        return strdup(node->name);
    }
    return strdup(node->name);
}

static char* resolveLValueName(struct parseTree* node) {
    char* name = extractIdentFromLValue(node);
    int addr = 0;
    if (findSymbolAddressByName(globalSymTable, name, &addr)) {
        const char* r = regByAddr(addr);
        if (r) {
            free(name);
            return strdup(r);
        }
        return name;
    } else {
        if (locals_lookup(name, &addr)) {
            const char* r = regByAddr(addr);
            if (r) {
                free(name);
                return strdup(r);
            }
            return name;
        }
    }
    return name;
}

// --- IO lowering helpers (forward declarations) ---
static void lower_io_out_chain(struct parseTree* n);
static void lower_io_in_chain(struct parseTree* n);
static void store_value_to_name(const char* dstName, const char* valueReg);

char* processParseTreeAndGenerateIR(struct parseTree *pt) {
    if (!pt) return NULL;
    printf("[PT] %s\n", pt->name);
    if (!pt->left && !pt->right) {
        return resolveLeafAtom(pt->name);
    }

    if (strcmp(pt->name, "OP_PLACE") == 0 && pt->left) {
        return resolveLeafAtom(pt->left->name);
    }

    if (strcmp(pt->name, "VarDeclToken") == 0 || strcmp(pt->name, "VarDecl") == 0) {
        struct parseTree* idNode   = pt->left;
        struct parseTree* exprNode = pt->right;
        exprNode = unwrapExpr(exprNode);

        char* varName = extractIdentFromLValue(idNode);
        char* rhs     = exprNode ? processParseTreeAndGenerateIR(exprNode) : strdup("0");

        int addr = 0;
        if (!locals_lookup(varName, &addr)) {
            addr = nextFreeAddress;
            nextFreeAddress += 2;
            locals_insert(varName, addr);
        }

        char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
        printf("[DECL] %s addr=%d\n", varName, addr);
        emit_store(rhs, strdup(buf));
        free(varName);
        return NULL;
    }

    if (strcmp(pt->name, "CallToken") == 0) {
        struct parseTree* id = pt->left;
        const char* fname = (id && id->name) ? id->name : NULL;
        struct parseTree* args = pt->right;

        char* argv[4] = {0};
        int argc = 0;

        struct parseTree* stack[32];
        int top = 0;
        if (args) stack[top++] = args;

        while (top > 0 && argc < 4) {
            struct parseTree* n = stack[--top];
            if (!n) continue;

            if (n->name && (strcmp(n->name, "ExpressionListToken") == 0 ||
                            strcmp(n->name, "ExpressionList") == 0)) {
                if (n->right) stack[top++] = n->right;
                if (n->left)  stack[top++] = n->left;
                continue;
                            }
            argv[argc++] = processParseTreeAndGenerateIR(n);
        }

        if (argc > 0) emit_mov("r0", argv[0]);
        if (argc > 1) emit_mov("r1", argv[1]);
        if (argc > 2) emit_mov("r2", argv[2]);
        if (argc > 3) emit_mov("r3", argv[3]);

        emit_call(fname);
        return strdup("r0");
    }

    if (strcmp(pt->name, "ReturnToken") == 0 ||
    strcmp(pt->name, "Return") == 0 ||
    strcmp(pt->name, "ReturnStmt") == 0 ||
    strncmp(pt->name, "Return", 6) == 0)
    {
        printf("[RET]\n");
        struct parseTree* exprNode = pt->left ? pt->left : pt->right;
        exprNode = unwrapExpr(exprNode);
        char* val = exprNode ? processParseTreeAndGenerateIR(exprNode) : strdup("0");
        emit_mov("r0", val);
        emit_jump(g_func_epilogue_label);
        return NULL;
    }
    if (strcmp(pt->name, "OutputToken") == 0) {
        struct parseTree* exprNode = unwrapExpr(pt->left ? pt->left : pt->right);
        char* val = exprNode ? processParseTreeAndGenerateIR(exprNode) : strdup("0");
        emit_mov("r0", val);
        emit_call("__print_u16");
        emit_call("__io_println");
        return NULL;
    }
    if (strcmp(pt->name, "IO_OUT") == 0) {
        lower_io_out_chain(pt);
        return NULL;
    }

    if (strcmp(pt->name, "InputToken") == 0) {
        struct parseTree* idNode = pt->left ? pt->left : pt->right;
        char* varName = extractIdentFromLValue(idNode);
        int addr;
        if (!locals_lookup(varName, &addr)) {
            int a = IO_ADDR_IN0 + 2 * g_io_in_count; // 4106, 4108, 4110, ...
            locals_insert(varName, a);
        }
        g_io_in_count++;
        free(varName);
        return NULL; // no input here; bootstrap does it
    }
    if (strcmp(pt->name, "OpInputToken") == 0) {
        struct parseTree* idNode = pt->left ? pt->left : pt->right;
        char* varName = extractIdentFromLValue(idNode);
        int dummy;
        if (!locals_lookup(varName, &dummy)) {
            locals_insert(varName, IO_ADDR_OP); // bind to 4104
        }
        free(varName);
        return NULL; // no runtime call here
    }

    if (strcmp(pt->name, "IO_IN") == 0) {
        return NULL;
    }
    if (isOperation(pt->name)) {
        if (!pt->left || !pt->right) {
            handleError(2, pt->name);
            return strdup("BAD_OP");
        }

        // Присваивание — оставь как у тебя (работает), ниже меняем только арифметику
        if (strcmp(pt->name, "OP_ASSIGN") == 0) {
            char* dst = resolveLValueName(pt->left);
            char* rhs = processParseTreeAndGenerateIR(pt->right);

            if (isRegName(dst)) {
                emit_mov(dst, rhs);
                return NULL;
            }
            int addr;
            if (findSymbolAddressByName(globalSymTable, dst, &addr)) {
                const char* r = regByAddr(addr);
                if (r) { emit_mov(r, rhs); return NULL; }
                if (addr > 0) {
                    char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
                    emit_store(rhs, strdup(buf));
                    return NULL;
                }
            }
            if (locals_lookup(dst, &addr)) {
                if (addr <= 0) { emit_store_fp(rhs, addr); return NULL; }
                char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
                emit_store(rhs, strdup(buf));
                return NULL;
            }
            addr = nextFreeAddress; nextFreeAddress += 2;
            locals_insert(dst, addr);
            { char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
                emit_store(rhs, strdup(buf)); }
            return NULL;
        }

        /*
        char* lhs = processParseTreeAndGenerateIR(pt->left);
        char* rhs = processParseTreeAndGenerateIR(pt->right);
        */
#if ENABLE_DYNAMIC_RUNTIME
        const char* fname = NULL;
        if      (strcmp(pt->name, "OP_ADD") == 0) fname = "__dyn_add";
        else if (strcmp(pt->name, "OP_SUB") == 0) fname = "__dyn_sub";
        else if (strcmp(pt->name, "OP_MUL") == 0) fname = "__dyn_mul";
        else if (strcmp(pt->name, "OP_DIV") == 0) fname = "__dyn_div";
        else if (strcmp(pt->name, "OP_MOD") == 0) fname = "__dyn_mod";
        emit_mov("r1", rhs);    // rhs → r1
        emit_mov("r0", lhs);    // lhs → r0
        emit_call(fname);
        char* result = strdup("r0");
#else
        char* lhs = processParseTreeAndGenerateIR(pt->left);
        char* rhs = processParseTreeAndGenerateIR(pt->right);

        // materialize: r1 = lhs, r0 = rhs
        emit_mov("r0", lhs);
        emit_push("r0");
        emit_mov("r0", rhs);
        emit_pop("r1");        // r1=lhs, r0=rhs

        // хотим r0=lhs, r1=rhs → свап через стек (без лишних mov-ов в асме)
        emit_push("r0");       // [ rhs ]
        emit_mov("r0", "r1");  // r0 = lhs
        emit_pop("r1");        // r1 = rhs

        if      (strcmp(pt->name, "OP_ADD") == 0) emit_add("r0","r0","r1"); // r0 += r1
        else if (strcmp(pt->name, "OP_SUB") == 0) emit_sub("r0","r0","r1"); // r0 -= r1
        else if (strcmp(pt->name, "OP_MUL") == 0) emit_mul("r0","r0","r1"); // r0 *= r1
        else if (strcmp(pt->name, "OP_DIV") == 0) emit_div("r0","r0","r1"); // r0 /= r1
        else if (strcmp(pt->name, "OP_MOD") == 0) emit_rem("r0","r0","r1"); // r0 %= r1

        return strdup("r0");
#endif
        if (pt->left)  processParseTreeAndGenerateIR(pt->left);
        if (pt->right) processParseTreeAndGenerateIR(pt->right);
        return NULL;
    }

    if (pt->left) {
        processParseTreeAndGenerateIR(pt->left);
    }
    if (pt->right) {
        processParseTreeAndGenerateIR(pt->right);
    }
    return NULL;
}
static void store_value_to_name(const char* dstName, const char* valueReg) {
    if (!dstName || !valueReg) return;

    if (isRegName(dstName)) {
        emit_mov(dstName, valueReg);
        return;
    }

    int addr = 0;
    if (findSymbolAddressByName(globalSymTable, dstName, &addr)) {
        const char* r = regByAddr(addr);
        if (r) {
            emit_mov(r, valueReg);
            return;
        }
        if (addr > 0) {
            char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
            emit_store(valueReg, strdup(buf));
            return;
        }
    }

    if (locals_lookup(dstName, &addr)) {
        if (addr <= 0) {
            emit_store_fp((char*)valueReg, addr);
            return;
        } else {
            char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
            emit_store(valueReg, strdup(buf));
            return;
        }
    }

    // Новый символ — выделяем свежий адрес
    addr = nextFreeAddress;
    nextFreeAddress += 2;
    locals_insert(dstName, addr);

    char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
    emit_store(valueReg, strdup(buf));
}

static void lower_io_out_chain(struct parseTree* n) {
    while (n) {
        const char* tag = n->name ? n->name : "";
        if (strcmp(tag, "IO_OUT") == 0) {
            struct parseTree* item = n->left;
            if (item && !item->left && !item->right &&
                item->name && strcmp(item->name, "endl") == 0) {
                emit_call("__io_println");
                } else {
                    char* val = processParseTreeAndGenerateIR(item);
                    emit_mov("r0", val);
                    emit_call("__print_u16");
                }
            n = n->right;
            continue;
        }
        if (strcmp(tag, "OutputToken") == 0) {
            struct parseTree* exprNode = unwrapExpr(n->left ? n->left : n->right);
            char* val = processParseTreeAndGenerateIR(exprNode);
            emit_mov("r0", val);
            emit_call("__print_u16");
            emit_call("__io_println");
            n = NULL;
            continue;
        }
        break;
    }
}

static void lower_io_in_chain(struct parseTree* n) {
    while (n) {
        const char* tag = n->name ? n->name : "";
        if (strcmp(tag, "IO_IN") == 0) {
            struct parseTree* item = n->left;
            char* varName = extractIdentFromLValue(item);
            int addr;
            if (!locals_lookup(varName, &addr)) {
                int a = IO_ADDR_IN0 + 2 * g_io_in_count;
                locals_insert(varName, a);
            }
            g_io_in_count++;
            free(varName);
            n = n->right;
            continue;
        }
        if (strcmp(tag, "InputToken") == 0) {
            struct parseTree* idNode = n->left;
            char* varName = extractIdentFromLValue(idNode);
            int addr;
            if (!locals_lookup(varName, &addr)) {
                int a = IO_ADDR_IN0 + 2 * g_io_in_count;
                locals_insert(varName, a);
            }
            g_io_in_count++;
            free(varName);
            n = NULL;
            continue;
        }
        break;
    }
}

static bool lowerCondFromParseTree(struct parseTree* pt, const char* labelIfFalse) {
    if (!pt || !pt->name) return false;
    const char* tag = pt->name;

    if (strcmp(tag, "OP_EQ")  && strcmp(tag, "OP_NEQ") &&
        strcmp(tag, "OP_LT")  && strcmp(tag, "OP_GT")  &&
        strcmp(tag, "OP_LE")  && strcmp(tag, "OP_GE")) {
        return false; // не сравнение
        }

    struct parseTree* L = unwrapExpr(pt->left);
    struct parseTree* R = unwrapExpr(pt->right);
    char* lhs = L ? processParseTreeAndGenerateIR(L) : strdup("0");
    char* rhs = R ? processParseTreeAndGenerateIR(R) : strdup("0");

    // r0 = lhs, r1 = rhs
    emit_mov("r0", lhs);
    emit_push("r0");
    emit_mov("r0", rhs);
    emit_pop("r1");        // r1=lhs, r0=rhs
    emit_push("r0");
    emit_mov("r0", "r1");  // r0=lhs
    emit_pop("r1");        // r1=rhs

    // r0 = lhs - rhs
    emit_sub("r0", "r0", "r1");

    if (strcmp(tag, "OP_EQ") == 0) {           // false if r0 != 0
        emit_jumpgt("r0", (char*)labelIfFalse);
        emit_jumplt("r0", (char*)labelIfFalse);
    } else if (strcmp(tag, "OP_NEQ") == 0) {   // false if r0 == 0
        emit_jumpeq("r0", (char*)labelIfFalse);
    } else if (strcmp(tag, "OP_GT") == 0) {    // lhs>rhs → r0>0 → false if r0<=0
        emit_jumpeq("r0", (char*)labelIfFalse);
        emit_jumplt("r0", (char*)labelIfFalse);
    } else if (strcmp(tag, "OP_LT") == 0) {    // lhs<rhs → r0<0 → false if r0>=0
        emit_jumpeq("r0", (char*)labelIfFalse);
        emit_jumpgt("r0", (char*)labelIfFalse);
    } else if (strcmp(tag, "OP_GE") == 0) {    // false if r0<0
        emit_jumplt("r0", (char*)labelIfFalse);
    } else if (strcmp(tag, "OP_LE") == 0) {    // false if r0>0
        emit_jumpgt("r0", (char*)labelIfFalse);
    }
    return true;
}

// compiler.c
void processCondExpression(const char* exprStr, const char* labelIfFalse) {
    if (!exprStr || !*exprStr) { emit_jump((char*)labelIfFalse); return; }

    // если нет операторов сравнения — трактуем как "truthy(expr)"
    if (!strpbrk(exprStr, "=!<>")) {
        char* atom = resolveLeafAtom(exprStr);
        emit_mov("r0", atom);
        // false если r0 == 0
        emit_jumpeq("r0", (char*)labelIfFalse);
        free(atom);
        return;
    }

    char lhs[64]={0}, rhs[64]={0}, op[8]={0};
    splitCondExpr(exprStr, lhs, op, rhs);

    char* L = resolveLeafAtom(lhs);
    char* R = resolveLeafAtom(rhs);

    // r0 = L, r1 = R
    emit_mov("r0", L);
    emit_push("r0");
    emit_mov("r0", R);
    emit_pop("r1");        // r1=L, r0=R
    emit_push("r0");
    emit_mov("r0", "r1");  // r0=L
    emit_pop("r1");        // r1=R
    emit_sub("r0", "r0", "r1"); // r0 = L - R

    if      (!strcmp(op, "==")) { emit_jumpgt("r0", (char*)labelIfFalse); emit_jumplt("r0", (char*)labelIfFalse); }
    else if (!strcmp(op, "!=")) { emit_jumpeq("r0", (char*)labelIfFalse); }
    else if (!strcmp(op,  ">"))  { emit_jumpeq("r0", (char*)labelIfFalse); emit_jumplt("r0", (char*)labelIfFalse); }
    else if (!strcmp(op,  "<"))  { emit_jumpeq("r0", (char*)labelIfFalse); emit_jumpgt("r0", (char*)labelIfFalse); }
    else if (!strcmp(op, ">="))  { emit_jumplt("r0", (char*)labelIfFalse); }
    else if (!strcmp(op, "<="))  { emit_jumpgt("r0", (char*)labelIfFalse); }
    else { emit_jumpeq("r0", (char*)labelIfFalse); } // дефолт: как truthy(r0)
}

char* extractExprFromParseTree(struct parseTree* pt) {
    if (!pt) return strdup("0");

    if (strcmp(pt->name, "ExpressionToken") == 0 || strcmp(pt->name, "Expression") == 0) {
        struct parseTree* child = pt->left ? pt->left : pt->right;
        return extractExprFromParseTree(child);
    }

    if (!pt->left && !pt->right) {
        return strdup(pt->name);
    }

    const char* op = NULL;
    if (strcmp(pt->name, "OP_GT") == 0) op = ">";
    else if (strcmp(pt->name, "OP_LT") == 0) op = "<";
    else if (strcmp(pt->name, "OP_EQ") == 0) op = "==";
    else if (strcmp(pt->name, "OP_NEQ") == 0) op = "!=";
    else if (strcmp(pt->name, "OP_GE") == 0) op = ">=";
    else if (strcmp(pt->name, "OP_LE") == 0) op = "<=";
    else if (strcmp(pt->name, "OP_ADD") == 0) op = "+";
    else if (strcmp(pt->name, "OP_SUB") == 0) op = "-";
    else if (strcmp(pt->name, "OP_MUL") == 0) op = "*";
    else if (strcmp(pt->name, "OP_DIV") == 0) op = "/";
    else if (strcmp(pt->name, "OP_ASSIGN") == 0) op = "=";

    if (op && pt->left && pt->right) {
        char* lhs = extractExprFromParseTree(pt->left);
        char* rhs = extractExprFromParseTree(pt->right);
        char* buf = malloc(strlen(lhs) + strlen(rhs) + strlen(op) + 4);
        sprintf(buf, "%s%s%s", lhs, op, rhs);
        free(lhs); free(rhs);
        return buf;
    }

    printf("[extractExpr] WARNING: unhandled node '%s'\n", pt->name);
    return strdup("BAD_EXPR");
}

static char* lowerSimpleArithExpr(const char* expr) {
    if (!expr) return strdup("0");
    const char* p = strchr(expr, '+');
    char op = 0;
    if (!p) { p = strchr(expr, '-'); op = '-'; } else { op = '+'; }
    if (!p) return resolveLeafAtom(expr);

    size_t L = (size_t)(p - expr);
    char lhs[128], rhs[128];
    if (L >= sizeof(lhs)-1) L = sizeof(lhs)-2;
    strncpy(lhs, expr, L); lhs[L] = 0;
    snprintf(rhs, sizeof(rhs), "%s", p + 1);
    char* l = lhs; while (*l==' '||*l=='\t') ++l;
    char* r = rhs; while (*r==' '||*r=='\t') ++r;

    char* la = resolveLeafAtom(l);
    char* ra = resolveLeafAtom(r);

    emit_mov("r0", la);
    emit_push("r0");
    emit_mov("r0", ra);
    emit_pop("r1");

    if (op=='+') emit_add("r1", "r1", "r0");
    else         emit_sub("r1", "r1", "r0");

    emit_mov("r0", "r1");
    return strdup("r0");
}

// --- Simple string expression parser for fallback lowering (calls + precedence) ---

static void skip_ws(const char** p) {
    while (*p && **p && isspace((unsigned char)**p)) (*p)++;
}

static int is_ident_start(char c) {
    return isalpha((unsigned char)c) || c == '_';
}


static char* parse_expr_str(const char** p);

static char* apply_binop_from_str(const char* op, char* a, char* b) {
#if ENABLE_DYNAMIC_RUNTIME
    const char* fname = NULL;
    if      (strcmp(op, "+") == 0) fname = "__dyn_add";
    else if (strcmp(op, "-") == 0) fname = "__dyn_sub";
    else if (strcmp(op, "*") == 0) fname = "__dyn_mul";
    else if (strcmp(op, "/") == 0) fname = "__dyn_div";
    else if (strcmp(op, "%") == 0) fname = "__dyn_mod";
    return call_dyn_binary(fname, a, b);
#else
    // NON-DYNAMIC: (a op b) → r0
    emit_mov("r0", a);
    emit_push("r0");
    emit_mov("r0", b);
    emit_pop("r1");

    if      (strcmp(op, "+") == 0) emit_add("r1", "r1", "r0");
    else if (strcmp(op, "-") == 0) emit_sub("r1", "r1", "r0");
    else if (strcmp(op, "*") == 0) emit_mul("r1", "r1", "r0");
    else if (strcmp(op, "/") == 0) emit_div("r1", "r1", "r0");
    else if (strcmp(op, "%") == 0) emit_rem("r1", "r1", "r0");

    emit_mov("r0", "r1");
    return strdup("r0");
#endif
}

static char* parse_identifier(const char** p) {
    const char* s = *p;
    while (**p && is_ident_char(**p)) (*p)++;
    size_t n = (size_t)(*p - s);
    char* id = (char*)malloc(n + 1);
    memcpy(id, s, n);
    id[n] = '\0';
    return id;
}

static char* parse_number(const char** p) {
    const char* s = *p;
    while (**p && isdigit((unsigned char)**p)) (*p)++;
    size_t n = (size_t)(*p - s);
    char* num = (char*)malloc(n + 1);
    memcpy(num, s, n);
    num[n] = '\0';
    return num;
}

static char* parse_primary_str(const char** p) {
    skip_ws(p);
    if (**p == '(') {
        (*p)++;
        char* v = parse_expr_str(p);
        skip_ws(p);
        if (**p == ')') (*p)++;
        return v;
    }
    if (is_ident_start(**p)) {
        char* id = parse_identifier(p);
        skip_ws(p);
        if (**p == '(') {
            // function call
            (*p)++;
            char* args[4] = {0};
            int argc = 0;
            skip_ws(p);
            if (**p != ')') {
                while (argc < 4) {
                    args[argc++] = parse_expr_str(p);
                    skip_ws(p);
                    if (**p == ',') { (*p)++; skip_ws(p); continue; }
                    break;
                }
            }
            skip_ws(p);
            if (**p == ')') (*p)++;

            if (argc > 0) emit_mov("r0", args[0]);
            if (argc > 1) emit_mov("r1", args[1]);
            if (argc > 2) emit_mov("r2", args[2]);
            if (argc > 3) emit_mov("r3", args[3]);

            emit_call(id);
            free(id);
            return strdup("r0");
        } else {
            char* atom = resolveLeafAtom(id);
            free(id);
            return atom;
        }
    }
    if (isdigit((unsigned char)**p)) {
        return parse_number(p);
    }
    // Fallback: treat as zero
    return strdup("0");
}

static char* parse_muldiv_str(const char** p) {
    char* left = parse_primary_str(p);
    for (;;) {
        skip_ws(p);
        char c = **p;
        if (c == '*' || c == '/' || c == '%') {
            (*p)++;
            char op[2] = { c, 0 };
            char* right = parse_primary_str(p);
            left = apply_binop_from_str(op, left, right);
        } else {
            break;
        }
    }
    return left;
}

static char* parse_addsub_str(const char** p) {
    char* left = parse_muldiv_str(p);
    for (;;) {
        skip_ws(p);
        char c = **p;
        if (c == '+' || c == '-') {
            (*p)++;
            char op[2] = { c, 0 };
            char* right = parse_muldiv_str(p);
            left = apply_binop_from_str(op, left, right);
        } else {
            break;
        }
    }
    return left;
}

static char* parse_expr_str(const char** p) {
    return parse_addsub_str(p);
}

static char* lowerExprFromString(const char* s) {
    if (!s) return strdup("0");
    const char* p = s;
    return parse_expr_str(&p);
}

void generateIRFromCFGNode(struct cfgNode* node) {
    if (!node || !node->name) return;
    if (node->visited) return;

    node->visited = true;

    if (node->name && strcmp(node->name, "OutputToken") == 0) {
        if (node->parseTree) {
            lower_io_out_chain(node->parseTree);
        }
        return;
    }
    if (node->name && strcmp(node->name, "InputToken") == 0) {
        if (node->parseTree) {
            lower_io_in_chain(node->parseTree);
        }
        return;
    }

    if (isReturnNode(node->name)) {
        // В main игнорируем голый 'return;' (без выражения), чтобы не делать ранний прыжок в эпилог
        if (g_is_main && (!node->parseTree)) {
            return; // просто не генерим ничего для пустого return в main
        }

        struct parseTree* exprNode = node->parseTree ? unwrapExpr(node->parseTree) : NULL;
        char* val = exprNode ? processParseTreeAndGenerateIR(exprNode) : strdup("0");
        emit_mov("r0", val);
        emit_jump(g_func_epilogue_label);
        return;
    }

    if (isCond(node->name)) {
        struct parseTree* condTree = NULL;
        if (node->parseTree) {
            condTree = unwrapExpr(node->parseTree);
        }

        char* L_else = allocLabel();
        char* L_end  = allocLabel();

        bool lowered = false;
        if (condTree) {
            lowered = lowerCondFromParseTree(condTree, L_else);
        }
        if (!lowered) {
            char* condExpr = condTree ? extractExprFromParseTree(condTree)
                                      : extractToken(node->name);
            processCondExpression(condExpr, L_else);
        }

        if (node->conditionalBranch)
            generateIRFromCFGNode(node->conditionalBranch);
        emit_jump(L_end);

        emit_label(L_else);
        if (node->defaultBranch)
            generateIRFromCFGNode(node->defaultBranch);

        emit_label(L_end);
        return;
    }

    if (isWhile(node->name)) {
        if (node->name && strcmp(node->name, "while_body") == 0) {
            return;
        }

        printf(">> while node ID: %d, name: %s\n", node->id, node->name);
        if (!node->parseTree) {
            printf("No parseTree in while\n");
            return;
        }

        printf("[while] loopRoot: %s\n", node->parseTree->name);
        if (node->parseTree->left)
            printf("[while] left: %s\n", node->parseTree->left->name);
        if (node->parseTree->right)
            printf("[while] right: %s\n", node->parseTree->right->name);

        char* labelCond = allocLoopLabel(1);
        char* labelExit = allocLoopLabel(2);

        emit_label(labelCond);

        struct parseTree* condTree = unwrapExpr(node->parseTree);
        if (!lowerCondFromParseTree(condTree, labelExit)) {
            char* exprStr = extractExprFromParseTree(condTree);
            printf("[while] Extracted expr: %s\n", exprStr);
            processCondExpression(exprStr, labelExit);
        }

        if (node->conditionalBranch)
            generateIRFromCFGNode(node->conditionalBranch);

        emit_jump(labelCond);
        emit_label(labelExit);
        return;
    }

    if (node->parseTree) {
        processParseTreeAndGenerateIR(node->parseTree);
    }

    if (node->conditionalBranch)
        generateIRFromCFGNode(node->conditionalBranch);
    if (node->defaultBranch)
        generateIRFromCFGNode(node->defaultBranch);
}


void collectGraphNodes(struct cfgNode *node, struct cfgNode **list, bool *used, int *count) {
    if (!node) return;
    if (used[node->id]) return;
    used[node->id] = true;
    list[*count] = node;
    (*count)++;
    collectGraphNodes(node->conditionalBranch, list, used, count);
    collectGraphNodes(node->defaultBranch, list, used, count);
}

void checkFullGraph(struct funcNode *fn) {
    if (!fn || !fn->cfgEntry) return;

    if (fn->identifier) {
        printf("[FUNC] label=%s\n", fn->identifier);
        emit_label(fn->identifier);
    } else {
        printf("[FUNC] label=<NULL>\n");
    }
    g_is_main = (fn->identifier && strcmp(fn->identifier, "func_0") == 0);
    locals_reset();
    g_func_epilogue_label = allocLabel();

    struct cfgNode *arr[4096];
    bool used[65536] = {0};
    int cnt = 0;
    collectGraphNodes(fn->cfgEntry, arr, used, &cnt);

    // Removed pre-planning of locals and stack frame allocation.

    for (int i = 0; i < cnt; i++) {
        generateIRFromCFGNode(arr[i]);
    }
    emit_label(g_func_epilogue_label);
    emit_ret();
}
void traverseGraph(struct programGraph *graph) {
    for (int i = 0; i < graph->funcCount; i++) {
        checkFullGraph(graph->functions[i]);
    }
}

void compile(pANTLR3_BASE_TREE* tree, struct programGraph *graph) {
    symbolTable* symTable = processTreeToBuild(tree);
    globalSymTable = symTable;
    traverseGraph(graph);
    IRInstruction** pool = get_pool();
    printf("[compile] IR count: %d\n", irCount);
    generateASM(pool, irCount);
}