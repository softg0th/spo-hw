#include <ctype.h>
#include "../types/typization.h"
#include "asm.h"
#include "../ir/errors.h"



#include "lib.h"
// Forward decls for helpers used before their definitions
char* allocTemp();

// ---- Dynamic typing runtime switch ----
// Set to 1 to route arithmetic/comparison through dynamic runtime helpers.
// This scaffolds "dynamic typing" without breaking current static codegen.
#ifndef ENABLE_DYNAMIC_RUNTIME
#define ENABLE_DYNAMIC_RUNTIME 1
#endif

#if ENABLE_DYNAMIC_RUNTIME
// Helper: call a dynamic runtime binary op: fname(lhs, rhs) -> r0 (result)
static char* call_dyn_binary(const char* fname, const char* lhsAtom, const char* rhsAtom) {
    // Move arguments into call registers (r0, r1), call helper, copy r0 to a fresh temp.
    if (lhsAtom) emit_mov("r0", (char*)lhsAtom); else emit_mov("r0", "0");
    if (rhsAtom) emit_mov("r1", (char*)rhsAtom); else emit_mov("r1", "0");
    emit_call(fname);
    char* t = allocTemp();
    emit_mov(t, "r0");
    return t;
}
#endif

symbolTable* globalSymTable;
extern int irCount;

typedef struct { char* name; int addr; } LocalSym;
static LocalSym gLocalSyms[1024];
static int gLocalSymCount = 0;

static void locals_reset(void) {
    for (int i = 0; i < gLocalSymCount; ++i) {
        free(gLocalSyms[i].name);
    }
    gLocalSymCount = 0;
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
            char* tmp = allocTemp();
            printf("[LOAD] name=%s addr=%d -> %s\n", name, addr, tmp);
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", addr);
            char* addrStr = strdup(buf);
            emit_load(addrStr, tmp);
            return tmp;
        }
    }

    // try local fallback map
    if (locals_lookup(name, &addr)) {
        const char* r = regByAddr(addr);
        if (r) {
            return strdup(r);
        } else if (addr > 0) {
            char* tmp = allocTemp();
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", addr);
            emit_load(strdup(buf), tmp);
            return tmp;
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
        // if not found in global table, check local fallback map
        if (locals_lookup(name, &addr)) {
            const char* r = regByAddr(addr);
            if (r) {
                free(name);
                return strdup(r);
            }
            // keep the plain name; the caller will compute/store by address
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
        // Locals-only allocation: don't touch the global symbol table for local vars
        if (!locals_lookup(varName, &addr)) {
            addr = nextFreeAddress;
            nextFreeAddress += 2;
            locals_insert(varName, addr);
        }

        char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
        printf("[DECL] %s addr=%d\n", varName, addr);
        emit_store(rhs, strdup(buf));  // store rhs -> [addr]
        free(varName);
        return NULL;
    }

    if (strcmp(pt->name, "CallToken") == 0) {
        struct parseTree* id = pt->left;
        const char* fname = (id && id->name) ? id->name : NULL;
        struct parseTree* args = pt->right;

        char* argv[4] = {0};
        int argc = 0;

        // Flatten up to 4 arguments from a binary tree/list node using an explicit stack
        struct parseTree* stack[32];
        int top = 0;
        if (args) stack[top++] = args;

        while (top > 0 && argc < 4) {
            struct parseTree* n = stack[--top];
            if (!n) continue;

            if (n->name && (strcmp(n->name, "ExpressionListToken") == 0 ||
                            strcmp(n->name, "ExpressionList") == 0)) {
                // push right then left to evaluate left-to-right
                if (n->right) stack[top++] = n->right;
                if (n->left)  stack[top++] = n->left;
                continue;
                            }

            // treat this subtree as one expression argument
            argv[argc++] = processParseTreeAndGenerateIR(n);
        }

        if (argc > 0) emit_mov("r0", argv[0]);
        if (argc > 1) emit_mov("r1", argv[1]);
        if (argc > 2) emit_mov("r2", argv[2]);
        if (argc > 3) emit_mov("r3", argv[3]);

        emit_call(fname);
        char* tmp = allocTemp();
        emit_mov(tmp, "r0");
        return tmp;
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

            if (isRegName(dst)) {          // r0..r3 (параметры)
                emit_mov(dst, rhs);
                return NULL;
            }

            int addr;
            if (!findSymbolAddressByName(globalSymTable, dst, &addr)) {
                if (!locals_lookup(dst, &addr)) {
                    addr = nextFreeAddress;
                    nextFreeAddress += 2;
                    locals_insert(dst, addr);
                }
            }
            // если нашли в глобальной — addr уже задан и мы его используем

            char buf[32];
            snprintf(buf, sizeof(buf), "%d", addr);
            emit_store(rhs, strdup(buf));
            return NULL;
        }

        char* lhs = processParseTreeAndGenerateIR(pt->left);
        char* rhs = processParseTreeAndGenerateIR(pt->right);
        // Compute arithmetic
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


    // Generic fallback: descend into children so we don't skip statements wrapped in nodes
    if (pt->left) {
        processParseTreeAndGenerateIR(pt->left);
    }
    if (pt->right) {
        processParseTreeAndGenerateIR(pt->right);
    }
    return NULL;
}

// Structural lowering for simple conditions (comparison parseTree nodes)
static bool lowerCondFromParseTree(struct parseTree* pt, const char* labelIfFalse) {
    if (!pt || !pt->name) return false;
    const char* tag = pt->name;

    // We only handle binary comparison nodes here.
    if (strcmp(tag, "OP_EQ") == 0 ||
        strcmp(tag, "OP_NEQ") == 0 ||
        strcmp(tag, "OP_LT") == 0 ||
        strcmp(tag, "OP_GT") == 0 ||
        strcmp(tag, "OP_LE") == 0 ||
        strcmp(tag, "OP_GE") == 0) {

        struct parseTree* L = unwrapExpr(pt->left);
        struct parseTree* R = unwrapExpr(pt->right);

        // Evaluate both sides to atoms (register/temp/immediate)
        char* lhs = L ? processParseTreeAndGenerateIR(L) : strdup("0");
        char* rhs = R ? processParseTreeAndGenerateIR(R) : strdup("0");

#if ENABLE_DYNAMIC_RUNTIME
        // Dynamic path: call runtime compare, which returns 0 (false) or 1 (true) in r0.
        const char* fname = NULL;
        if      (strcmp(tag, "OP_EQ") == 0)  fname = "__dyn_eq";
        else if (strcmp(tag, "OP_NEQ") == 0) fname = "__dyn_neq";
        else if (strcmp(tag, "OP_LT") == 0)  fname = "__dyn_lt";
        else if (strcmp(tag, "OP_GT") == 0)  fname = "__dyn_gt";
        else if (strcmp(tag, "OP_LE") == 0)  fname = "__dyn_le";
        else if (strcmp(tag, "OP_GE") == 0)  fname = "__dyn_ge";

        char* tbool = call_dyn_binary(fname, lhs, rhs);
        // Jump to else when condition is false (i.e., tbool == 0)
        emit_cond_jump_false(tbool, (char*)labelIfFalse);
        return true;
#else
        // Static int path: lhs - rhs in tcond, then branch by sign/zero.
        // Compute lhs - rhs into a fresh temp (do NOT use r0)
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
        // Treat as "if (atom)"; jump when atom is falsy
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
    // Dynamic compare via runtime helpers: __dyn_eq/__dyn_neq/__dyn_lt/__dyn_gt/__dyn_le/__dyn_ge
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

// Lower very simple "a+b" / "a-b" strings that may arrive from token text
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
    // trim leading spaces (simple)
    char* l = lhs; while (*l==' '||*l=='\t') ++l;
    char* r = rhs; while (*r==' '||*r=='\t') ++r;

    char* la = resolveLeafAtom(l);
    char* ra = resolveLeafAtom(r);
    char* t  = allocTemp();
    if (op=='+') emit_add(t, la, ra); else emit_sub(t, la, ra);
    return t;
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
        char* atom = lowerSimpleArithExpr(expr);
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

        // Prefer structural lowering (no fragile string parsing)
        bool lowered = false;
        if (condTree) {
            lowered = lowerCondFromParseTree(condTree, L_else);
        }
        if (!lowered) {
            char* condExpr = condTree ? extractExprFromParseTree(condTree)
                                      : extractToken(node->name);
            processCondExpression(condExpr, L_else);
        }

        // ---- THEN ----
        if (node->conditionalBranch)
            generateIRFromCFGNode(node->conditionalBranch);
        emit_jump(L_end);

        // ---- ELSE ----
        emit_label(L_else);
        if (node->defaultBranch)
            generateIRFromCFGNode(node->defaultBranch);

        // ---- END ----
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

    // reset per-function local fallback map
    locals_reset();

    struct cfgNode *arr[4096];
    bool used[65536];
    memset(used, 0, sizeof(used));
    int cnt = 0;
    collectGraphNodes(fn->cfgEntry, arr, used, &cnt);

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