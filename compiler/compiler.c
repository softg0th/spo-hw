#include <ctype.h>
#include "../types/typization.h"
#include "asm.h"
#include "../ir/errors.h"

#include "lib.h"

symbolTable* globalSymTable;
extern int irCount;


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
        bool haveAddr = findSymbolAddressByName(globalSymTable, varName, &addr);
        if (!haveAddr || addr <= 0) {
            if (!insertSymbolAddress(globalSymTable, varName, nextFreeAddress)) exit(1);
            addr = nextFreeAddress;
            nextFreeAddress += 2;
        }

        char buf[32]; snprintf(buf, sizeof(buf), "%d", addr);
        printf("[DECL] %s addr=%d\n", varName, addr);
        emit_store(rhs, strdup(buf));  // store rhs -> [addr]
        free(varName);
        return NULL;
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

            // If destination is a hardware register (parameter), simple MOV is fine
            if (isRegName(dst)) {
                emit_mov(dst, rhs);
                return NULL;
            }

            int addr = -999999;
            bool haveAddr = findSymbolAddressByName(globalSymTable, dst, &addr);
            if (!haveAddr || addr <= 0) {
                bool success = insertSymbolAddress(globalSymTable, dst, nextFreeAddress);
                if (!success) {
                    exit(1);
                }
                addr = nextFreeAddress;
                nextFreeAddress += 2;
            }

            char buf[32];
            snprintf(buf, sizeof(buf), "%d", addr);
            char* addrStr = strdup(buf);
            printf("[STORE] from=%s ptr=%s\n", rhs, addrStr);
            emit_store(rhs, addrStr);
            return NULL;
        }

        char* lhs = processParseTreeAndGenerateIR(pt->left);
        char* rhs = processParseTreeAndGenerateIR(pt->right);
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

void processCondExpression(const char* exprStr, const char* labelIfFalse) {
    char lhs[64] = {0}, rhs[64] = {0}, op[8] = {0};
    if (!exprStr) {
        emit_jump(labelIfFalse);
        return;
    }

    if (strlen(exprStr) == 0 || !strpbrk(exprStr, "=!<>")) {
        if (atoi(exprStr) == 0) {
            emit_jump(labelIfFalse);
        }
        return;
    }
    splitCondExpr(exprStr, lhs, op, rhs);

    char* lhsAtom = resolveLeafAtom(lhs);
    char* rhsAtom = resolveLeafAtom(rhs);

    emit_mov("r1", rhsAtom);
    emit_sub("r0", lhsAtom, "r1");
    free(lhsAtom);
    free(rhsAtom);


    if (strcmp(op, "==") == 0) {
        emit_cond_jump_false("r0", labelIfFalse);
    } else if (strcmp(op, "!=") == 0) {
        char* labelSkip = allocLabel();
        emit_cond_jump_false("r0", labelSkip);
        emit_jump(labelIfFalse);
        emit_label(labelSkip);
    } else if (strcmp(op, ">") == 0) {
        emit_jumpgt("r0", labelIfFalse);
    } else if (strcmp(op, "<") == 0) {
        emit_jumplt("r0", labelIfFalse);
    }

}

char* extractExprFromParseTree(struct parseTree* pt) {
    if (!pt) return strdup("0");

    if (!pt->left && !pt->right) {
        return strdup(pt->name);
    }

    const char* op = NULL;
    if (strcmp(pt->name, "OP_GT") == 0) op = ">";
    else if (strcmp(pt->name, "OP_LT") == 0) op = "<";
    else if (strcmp(pt->name, "OP_EQ") == 0) op = "==";
    else if (strcmp(pt->name, "OP_NEQ") == 0) op = "!=";
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

void generateIRFromCFGNode(struct cfgNode* node) {
    if (!node || !node->name) return;
    if (node->visited) return;

    node->visited = true;

    if (isReturnNode(node->name)) {
        if (node->parseTree) {
            struct parseTree* t = unwrapExpr(node->parseTree);
            char* val = processParseTreeAndGenerateIR(node->parseTree);
            emit_mov("r0", val);
            emit_ret();
            return;
        }
        char* expr = extractToken(node->name);
        char* atom = resolveLeafAtom(expr);
        emit_mov("r0", atom);
        emit_ret();
        return;
    }

    if (isCond(node->name)) {
        char* condExpr = extractToken(node->name);
        char* labelThen = allocLabel();
        char* labelElse = allocLabel();
        char* labelEnd  = allocLabel();

        processCondExpression(condExpr, labelElse);
        emit_jump(labelThen);

        emit_label(labelThen);
        if (node->conditionalBranch)
            generateIRFromCFGNode(node->conditionalBranch);
        emit_jump(labelEnd);

        emit_label(labelElse);
        if (node->defaultBranch)
            generateIRFromCFGNode(node->defaultBranch);

        emit_label(labelEnd);
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

        char* exprStr = extractExprFromParseTree(node->parseTree);
        printf("[while] Extracted expr: %s\n", exprStr);

        processCondExpression(exprStr, labelExit);

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
    if (!fn || !fn->cfgEntry)
        return;

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