#include "../graph/graphForExternal.h"
#include "utils.h"

#include <stdio.h>

struct typeError *errorList = NULL;
listOfTables* list = NULL;
symbolTable* currentTable = NULL;
dataType currentOpPseudoType = UNKNOWN_TYPE;
bool hasErrors = false;
bool singleVar = false;
static struct parseTree* primaryEntryPoint = NULL;

void checkForTypeError(dataType opType, char* opName) {
    if (currentOpPseudoType != UNKNOWN_TYPE && (opType != currentOpPseudoType)) {
        addTypeError(&errorList, opName, currentOpPseudoType);
        hasErrors = true;
    }
}

void primaryProcessParseTree(struct parseTree *parseTree) {
    bool binopStatus = isBinop(parseTree->name);
    if (!(parseTree->left) || !(parseTree->right) && binopStatus) {
        dataType opType = detectType(parseTree->name);
        currentOpPseudoType = opType;
        checkForTypeError(opType, parseTree->name);
        return;
    }
    if (strcmp(parseTree->name, "OP_ASSIGN") == 0) {
        primaryEntryPoint = parseTree;
        primaryProcessParseTree(parseTree->right);
    } else {
        if (currentOpPseudoType == UNKNOWN_TYPE) {
            currentOpPseudoType = detectType(parseTree -> right -> name);
        } else {
            dataType opType = detectType(parseTree->right->name);
            checkForTypeError(opType, parseTree->right->name);
        }
        primaryProcessParseTree(parseTree->left);
    }
}

void processReservedOP(struct cfgNode *node) {
    if (strcmp(node->name, "OP_ASSIGN") == 0) {
        primaryProcessParseTree(node->parseTree);
        if (!hasErrors) {
            if (primaryEntryPoint->right->right != NULL) {
                appendSymbolTable(currentTable, primaryEntryPoint->right->right->name, currentOpPseudoType);
            } else {
                appendSymbolTable(currentTable, primaryEntryPoint->right->name, currentOpPseudoType);
            }
            primaryEntryPoint = NULL;
            currentOpPseudoType = UNKNOWN_TYPE;
        }
    }
}

void processGraphFunc(struct funcNode *fn) {
    if (!fn || !fn->cfgEntry)
        return;
    currentTable = initSymbolTable(list, "temp");
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
    list = initListOfTables(graph->funcCount);
    for (int i = 0; i < graph->funcCount; i++) {
        processGraphFunc(graph->functions[i]);
    }

    if (errorList != NULL) {
        printErrors(errorList);
    }
}
