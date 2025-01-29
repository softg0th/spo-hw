#include "../graph/graph.h"
//#include "utils.c"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

bool isInteger(char *str) {
    if (*str == '\0') return false;

    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

bool isFloat(char *str) {
    char *endptr;
    strtod(str, &endptr);
    return *endptr == '\0';
}

char* detectType(char *value) {
    if (isInteger(value)) {
        return "integer";
    } else if (isFloat(value)) {
        return "float";
    } else {
        return "unknown";
    }
}

char* currentOpPseudoType = NULL;

bool isPrimaryProcessParseTreeSucceed(struct parseTree *parseTree){
    if (!(parseTree->left) || !(parseTree->right)) {
        return parseTree->name;
    }
    if (strcmp(parseTree->name, "op")) {
        isPrimaryProcessParseTreeSucceed(parseTree->right);
    } else {
        if (currentOpPseudoType == NULL) {
            currentOpPseudoType = detectType(parseTree -> name);
            if (strcmp(currentOpPseudoType, "unknown") == 0) {
                return false;
            }
        } else {
            if (strcmp(currentOpPseudoType, detectType(parseTree->name) != 0)) {
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
            i++;
        }
    }

}

void processGraphToBuild(struct programGraph *graph) {
    if (!graph) return;
    for (int i = 0; i < graph->funcCount; i++) {
        processGraphFunc(graph->functions[i]);
    }
}