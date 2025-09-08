#include <ctype.h>
#include "../types/typization.h"
#include "asm.h"
#include "../ir/errors.h"



#include "lib.h"

char* allocTemp();

#ifndef ENABLE_DYNAMIC_RUNTIME
#define ENABLE_DYNAMIC_RUNTIME 1
#endif

static int gFrameSize = 0;

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

static void locals_insert(const char* name, int addr) {
    if (!name) return;
    if (gLocalSymCount < (int)(sizeof(gLocalSyms)/sizeof(gLocalSyms[0]))) {
        gLocalSyms[gLocalSymCount].name = strdup(name);
        gLocalSyms[gLocalSymCount].addr = addr;
        ++gLocalSymCount;
    }
}

static void plan_locals_in_tree(struct parseTree* pt) {
    if (!pt) return;
    if (strcmp(pt->name, "VarDecl") == 0 || strcmp(pt->name, "VarDeclToken") == 0) {
        struct parseTree* idNode = pt->left;
        if (idNode) {
            char* varName = idNode->name ? strdup(idNode->name) : NULL;
            if (varName) {
                int dummy;
                if (!locals_lookup(varName, &dummy)) {
                    gFrameSize += 2;
                    locals_insert(varName, -gFrameSize);
                }
                free(varName);
            }
        }
    }
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
int nextFreeAddress = 0x1000;


char* allocTemp() {
    char* buf = malloc(10);
    sprintf(buf, "t%d", tempCounter++);
    return buf;
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

static bool isRegName(const char* s) {
    return s && (
        strcmp(s, "r0") == 0 ||
        strcmp(s, "r1") == 0 ||
        strcmp(s, "r2") == 0 ||
        strcmp(s, "r3") == 0
    );
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

static struct parseTree* unwrapExpr(struct parseTree* n) {
    if (!n) return NULL;
    if (strcmp(n->name, "ExpressionToken") == 0 || strcmp(n->name, "Expression") == 0) {
        return n->left ? n->left : n->right;
    }
    return n;
}

static char* resolveLeafAtom(const char* name) {
    if (!name) return strdup("0");

    if (isRegName(name) || (name[0] == 't' && isdigit((unsigned char)name[1]))) {
        return strdup(name);
    }

    int addr = 0;
    if (findSymbolAddressByName(globalSymTable, name, &addr)) {
        const char* r = regByAddr(addr);
        if (r) {
            return strdup(r);
        } else if (addr > 0) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", addr);
            char* addrStr = strdup(buf);
            emit_load(addrStr, "r0");
            return strdup("r0");
        }
    }

    if (locals_lookup(name, &addr)) {
        const char* r = regByAddr(addr);
        if (r) {
            return strdup(r);
        } else {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", addr);
            emit_load(strdup(buf), "r0");
            return strdup("r0");
        }
    }
    return strdup(name);
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
        emit_ret();
        return NULL;
    }

    if (isOperation(pt->name)) {
        if (!pt->left || !pt->right) {
            handleError(2, pt->name);
            return strdup("BAD_OP");
        }
        if (strcmp(pt->name, "OP_ASSIGN") == 0) {
            char* dst = resolveLValueName(pt->left);
            char* rhs = processParseTreeAndGenerateIR(pt->right);

            // Параметры в r0..r3
            if (isRegName(dst)) {
                emit_mov(dst, rhs);
                return NULL;
            }

            int addr;
            // Сначала пытаемся найти как глобал/параметр в текущей таблице
            if (findSymbolAddressByName(globalSymTable, dst, &addr)) {
                const char* r = regByAddr(addr);
                if (r) {
                    emit_mov(r, rhs); // крайне редкий случай, если сюда попал параметр
                    return NULL;
                }
                if (addr > 0) {
                    char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
                    emit_store(rhs, strdup(buf));
                    return NULL;
                }
                // если addr <= 0 и не r0..r3 — сюда мы не должны попадать
            }

            // Локальная переменная (офсет от fp)
            if (locals_lookup(dst, &addr)) {
                if (addr <= 0) {
                    emit_store_fp(rhs, addr);
                    return NULL;
                } else {
                    // редкий случай: абсолютный адрес — оставим старое поведение
                    char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
                    emit_store(rhs, strdup(buf));
                    return NULL;
                }
            }

            // Фоллбек: создадим «глобал» по абсолютному адресу (старое поведение)
            addr = nextFreeAddress;
            nextFreeAddress += 2;
            locals_insert(dst, addr);
            {
                char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
                emit_store(rhs, strdup(buf));
            }
            return NULL;
        }

        char* lhs = processParseTreeAndGenerateIR(pt->left);
        char* rhs = processParseTreeAndGenerateIR(pt->right);
#if ENABLE_DYNAMIC_RUNTIME
        const char* fname = NULL;
        if      (strcmp(pt->name, "OP_ADD") == 0) fname = "__dyn_add";
        else if (strcmp(pt->name, "OP_SUB") == 0) fname = "__dyn_sub";
        else if (strcmp(pt->name, "OP_MUL") == 0) fname = "__dyn_mul";
        else if (strcmp(pt->name, "OP_DIV") == 0) fname = "__dyn_div";
        else if (strcmp(pt->name, "OP_MOD") == 0) fname = "__dyn_mod";

        char* result = call_dyn_binary(fname, lhs, rhs);
#else
        char* result = allocTemp();
        if (strcmp(pt->name, "OP_ADD") == 0) {
            emit_add(result, lhs, rhs);
        } else if (strcmp(pt->name, "OP_SUB") == 0) {
            emit_sub(result, lhs, rhs);
        } else if (strcmp(pt->name, "OP_MUL") == 0) {
            emit_mul(result, lhs, rhs);
        } else if (strcmp(pt->name, "OP_DIV") == 0) {
            emit_div(result, lhs, rhs);
        } else if (strcmp(pt->name, "OP_MOD") == 0) {
            emit_rem(result, lhs, rhs);
        }
#endif
        return result;
    }

    if (pt->left) {
        processParseTreeAndGenerateIR(pt->left);
    }
    if (pt->right) {
        processParseTreeAndGenerateIR(pt->right);
    }
    return NULL;
}

static bool lowerCondFromParseTree(struct parseTree* pt, const char* labelIfFalse) {
    if (!pt || !pt->name) return false;
    const char* tag = pt->name;

    if (strcmp(tag, "OP_EQ") == 0 ||
        strcmp(tag, "OP_NEQ") == 0 ||
        strcmp(tag, "OP_LT") == 0 ||
        strcmp(tag, "OP_GT") == 0 ||
        strcmp(tag, "OP_LE") == 0 ||
        strcmp(tag, "OP_GE") == 0) {

        struct parseTree* L = unwrapExpr(pt->left);
        struct parseTree* R = unwrapExpr(pt->right);

        char* lhs = L ? processParseTreeAndGenerateIR(L) : strdup("0");
        char* rhs = R ? processParseTreeAndGenerateIR(R) : strdup("0");

#if ENABLE_DYNAMIC_RUNTIME
        const char* fname = NULL;
        if      (strcmp(tag, "OP_EQ") == 0)  fname = "__dyn_eq";
        else if (strcmp(tag, "OP_NEQ") == 0) fname = "__dyn_neq";
        else if (strcmp(tag, "OP_LT") == 0)  fname = "__dyn_lt";
        else if (strcmp(tag, "OP_GT") == 0)  fname = "__dyn_gt";
        else if (strcmp(tag, "OP_LE") == 0)  fname = "__dyn_le";
        else if (strcmp(tag, "OP_GE") == 0)  fname = "__dyn_ge";

        char* tbool = call_dyn_binary(fname, lhs, rhs);
        emit_cond_jump_false(tbool, (char*)labelIfFalse);
        return true;
#else
        char* tcond = allocTemp();
        emit_sub(tcond, lhs, rhs);

        if (strcmp(tag, "OP_EQ") == 0) {
            // false when tcond != 0
            emit_jumpgt(tcond, labelIfFalse);
            emit_jumplt(tcond, labelIfFalse);
        } else if (strcmp(tag, "OP_NEQ") == 0) {
            // false when tcond == 0
            emit_cond_jump_false(tcond, labelIfFalse);
        } else if (strcmp(tag, "OP_GT") == 0) {
            // false when tcond <= 0  → (==0) or (<0)
            emit_cond_jump_false(tcond, labelIfFalse);
            emit_jumplt(tcond, labelIfFalse);
        } else if (strcmp(tag, "OP_LT") == 0) {
            // false when tcond >= 0  → (==0) or (>0)
            emit_cond_jump_false(tcond, labelIfFalse);
            emit_jumpgt(tcond, labelIfFalse);
        } else if (strcmp(tag, "OP_GE") == 0) {
            // false when tcond < 0
            emit_jumplt(tcond, labelIfFalse);
        } else if (strcmp(tag, "OP_LE") == 0) {
            // false when tcond > 0
            emit_jumpgt(tcond, labelIfFalse);
        }

        return true;
#endif
    }

    return false;
}

void processCondExpression(const char* exprStr, const char* labelIfFalse) {
    char lhs[64] = {0}, rhs[64] = {0}, op[8] = {0};
    if (!exprStr) {
        emit_jump((char*)labelIfFalse);
        return;
    }

    if (strlen(exprStr) == 0) {
        emit_jump((char*)labelIfFalse);
        return;
    }
    if (!strpbrk(exprStr, "=!<>")) {
        char* endptr = NULL;
        long v = strtol(exprStr, &endptr, 10);
        if (endptr && *endptr == '\0') {
            if (v == 0) {
                emit_jump((char*)labelIfFalse);
            }
            return;
        }
        char* atom = resolveLeafAtom(exprStr);
#if ENABLE_DYNAMIC_RUNTIME
        char* tbool = call_dyn_binary("__dyn_truthy", atom, NULL);
        emit_cond_jump_false(tbool, (char*)labelIfFalse);
#else
        char* tcond = allocTemp();
        emit_mov(tcond, atom);
        emit_cond_jump_false(tcond, (char*)labelIfFalse);
#endif
        free(atom);
        return;
    }
    splitCondExpr(exprStr, lhs, op, rhs);

#if ENABLE_DYNAMIC_RUNTIME
    char* lhsAtom = resolveLeafAtom(lhs);
    char* rhsAtom = resolveLeafAtom(rhs);

    const char* fname = NULL;
    if      (strcmp(op, "==") == 0) fname = "__dyn_eq";
    else if (strcmp(op, "!=") == 0) fname = "__dyn_neq";
    else if (strcmp(op, "<")  == 0) fname = "__dyn_lt";
    else if (strcmp(op, ">")  == 0) fname = "__dyn_gt";
    else if (strcmp(op, "<=") == 0) fname = "__dyn_le";
    else if (strcmp(op, ">=") == 0) fname = "__dyn_ge";

    char* tbool = call_dyn_binary(fname, lhsAtom, rhsAtom);
    emit_cond_jump_false(tbool, (char*)labelIfFalse);
    free(lhsAtom);
    free(rhsAtom);
    return;
#else
    char* lhsAtom = resolveLeafAtom(lhs);
    char* rhsAtom = resolveLeafAtom(rhs);

    char* tmp = allocTemp();
    emit_mov(tmp, rhsAtom);
    char* tcond = allocTemp();
    emit_sub(tcond, lhsAtom, tmp);
    free(lhsAtom);
    free(rhsAtom);

    if (strcmp(op, "==") == 0) {
        emit_jumpgt(tcond, (char*)labelIfFalse);
        emit_jumplt(tcond, (char*)labelIfFalse);
    } else if (strcmp(op, "!=") == 0) {
        emit_cond_jump_false(tcond, (char*)labelIfFalse);
    } else if (strcmp(op, ">") == 0) {
        emit_cond_jump_false(tcond, (char*)labelIfFalse);
        emit_jumplt(tcond, (char*)labelIfFalse);
    } else if (strcmp(op, "<") == 0) {
        emit_cond_jump_false(tcond, (char*)labelIfFalse);
        emit_jumpgt(tcond, (char*)labelIfFalse);
    } else if (strcmp(op, ">=") == 0) {
        emit_jumplt(tcond, (char*)labelIfFalse);
    } else if (strcmp(op, "<=") == 0) {
        emit_jumpgt(tcond, (char*)labelIfFalse);
    } else {
        emit_cond_jump_false(tcond, (char*)labelIfFalse);
    }
#endif
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
    char* t  = allocTemp();
    if (op=='+') emit_add(t, la, ra); else emit_sub(t, la, ra);
    return t;
}

// --- Simple string expression parser for fallback lowering (calls + precedence) ---

static void skip_ws(const char** p) {
    while (*p && **p && isspace((unsigned char)**p)) (*p)++;
}

static int is_ident_start(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

static int is_ident_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
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
    char* t = allocTemp();
    if      (strcmp(op, "+") == 0) emit_add(t, a, b);
    else if (strcmp(op, "-") == 0) emit_sub(t, a, b);
    else if (strcmp(op, "*") == 0) emit_mul(t, a, b);
    else if (strcmp(op, "/") == 0) emit_div(t, a, b);
    else if (strcmp(op, "%") == 0) emit_rem(t, a, b);
    return t;
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

    if (isReturnNode(node->name)) {
        if (node->parseTree) {
            struct parseTree* t = unwrapExpr(node->parseTree);
            char* val = processParseTreeAndGenerateIR(t);
            emit_mov("r0", val);
            emit_ret();
            return;
        }
        char* expr = extractToken(node->name);
        char* atom = lowerExprFromString(expr);
        emit_mov("r0", atom);
        emit_ret();
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

    locals_reset();

    struct cfgNode *arr[4096];
    bool used[65536] = {0};
    int cnt = 0;
    collectGraphNodes(fn->cfgEntry, arr, used, &cnt);

    // Removed pre-planning of locals and stack frame allocation.

    for (int i = 0; i < cnt; i++) {
        generateIRFromCFGNode(arr[i]);
    }
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