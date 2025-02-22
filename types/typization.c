#include "../graph/graphForExternal.h"
#include "utils.h"
#include "errors.h"

#include <stdio.h>

struct typeError *errorList = NULL;
char* currentOpPseudoType = NULL;

void checkForTypeError(char* opType, char* opName) {
    if (currentOpPseudoType != NULL && strcmp(opType, currentOpPseudoType) != 0) {
        addTypeError(&errorList, opName, currentOpPseudoType);
    }
}

void primaryProcessParseTree(struct parseTree *parseTree) {
    if (!(parseTree->left) || !(parseTree->right)) {
        char* opType = detectType(parseTree->name);
        checkForTypeError(opType, parseTree->name);
        return;
    }
    if (strcmp(parseTree->name, "op") == 0) {
        primaryProcessParseTree(parseTree->right);
    } else {
        if (currentOpPseudoType == NULL) {
            currentOpPseudoType = detectType(parseTree -> right -> name);
        } else {
            char* opType = detectType(parseTree->right->name);
            checkForTypeError(opType, parseTree->right->name);
        }
        primaryProcessParseTree(parseTree->left);
    }
}

void processReservedOP(struct cfgNode *node) {
    if (strcmp(node->name, "OP_ASSIGN") == 0) {
        primaryProcessParseTree(node->parseTree);
    } else {
        printf("error");
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

    if (errorList != NULL) {
        printErrors(errorList);
    }
}
