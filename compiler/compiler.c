#include "../types/typization.h"
#include "../ir/ir.h"

#include "lib.h"

symbolTable* globalSymTable;

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

char* processParseTreeAndGenerateIR(struct parseTree *pt) {
    if (!pt) return NULL;

    if (!pt->left && !pt->right) {
        return strdup(pt->name);
    }

    if (strcmp(pt->name, "OP_PLACE") == 0 && pt->left) {
        return strdup(pt->left->name);
    }

    if (isOperation(pt->name)) {
        if (strcmp(pt->name, "OP_ASSIGN") == 0) {
            char* dst = processParseTreeAndGenerateIR(pt->left);
            char* rhs = processParseTreeAndGenerateIR(pt->right);
            bool success = insertSymbolAddress(globalSymTable, dst, nextFreeAddress);

            if (!success) {
                exit(1);
            }

            nextFreeAddress += 2;
            emit_mov(dst, rhs);
            return NULL;
        }

        char* lhs = processParseTreeAndGenerateIR(pt->left);
        char* rhs = processParseTreeAndGenerateIR(pt->right);
        char* result = allocTemp();

        if (strcmp(pt->name, "OP_ADD") == 0) {
            printf("add %s %s %s\n", result, lhs, rhs);
            emit_add(result, lhs, rhs);
        } else if (strcmp(pt->name, "OP_SUB") == 0) {
            printf("sub %s %s %s\n", result, lhs, rhs);
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

    return strdup(pt->name);
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
    collectNodes(fn->cfgEntry, arr, used, &cnt);

    for (int i = 0; i < cnt; i++) {
        bool interesting = isInterestingNode(arr[i] -> name);
        if (interesting) {
            if (arr[i] -> isParseTreeRoot) {
                processParseTreeAndGenerateIR(arr[i]->parseTree);
            }
        }
    }
}

void traverseGraph(struct programGraph *graph) {
    for (int i = 0; i < graph->funcCount; i++) {
        checkFullGraph(graph->functions[i]);
    }
}

void compile(pANTLR3_BASE_TREE tree, struct programGraph *graph) {
    symbolTable* symTable = processTreeToBuild(tree);
    globalSymTable = symTable;
    traverseGraph(graph);
    IRInstruction* pool = get_pool();

}