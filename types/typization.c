#include "../graph/graphForExternal.h"
#include "utils.h"

#include <stdio.h>

char* currentOpPseudoType = NULL;

bool isPrimaryProcessParseTreeSucceed(struct parseTree *parseTree){
    if (!(parseTree->left) || !(parseTree->right)) {
        return parseTree->name;
    }
    if (strcmp(parseTree->name, "op") == 0) {
        isPrimaryProcessParseTreeSucceed(parseTree->right);
    } else {
        if (currentOpPseudoType == NULL) {
            currentOpPseudoType = detectType(parseTree -> name);
            if (strcmp(currentOpPseudoType, "unknown") == 0) {
                printf("Unknown type %s\n", parseTree->name);
                return false;
            }
        } else {
            char* opName = detectType(parseTree->name);
            if (strcmp(currentOpPseudoType, opName) != 0) {
                printf("Unknown type %s\n", parseTree->name);
                return false;
            }
        }
        isPrimaryProcessParseTreeSucceed(parseTree->left);
    }
}

void processReservedOP(struct cfgNode *node) {
    if (strcmp(node->name, "OP_ASSIGN") == 0) {
        bool test = isPrimaryProcessParseTreeSucceed(node->parseTree);
        if (test) {
            printf("current op type: %s\n", currentOpPseudoType);
        }
    } else {
        return;
    }
}

void processGraphFunc(struct funcNode *fn) {
    if (!fn || !fn->cfgEntry)
        return;
    struct cfgNode *arr[4096];
    bool used[65536];
    memset(used, 0, sizeof(used));
    int cnt = 0;
    collectNodes(fn->cfgEntry, arr, used, &cnt);
    int i = 0;
    while (i < cnt) {
        struct cfgNode *nd = arr[i];
        if (nd->isParseTreeRoot) {
            processReservedOP(nd);
        }
        i++;
    }

}

void processGraphToBuild(struct programGraph *graph) {
    if (!graph) return;
    for (int i = 0; i < graph->funcCount; i++) {
        processGraphFunc(graph->functions[i]);
    }
}
