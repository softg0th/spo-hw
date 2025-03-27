#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct typeError *errorList = NULL;
listOfTables* list = NULL;
symbolTable* currentTable = NULL;
listOfVariables* currentVariables = NULL;
dataType currentOpPseudoType = UNKNOWN_TYPE;
bool hasErrors = false;

void checkForTypeError(dataType opType, char* opName) {
    if (currentOpPseudoType != UNKNOWN_TYPE && (opType != currentOpPseudoType)) {
        addTypeError(&errorList, opName, currentOpPseudoType);
        hasErrors = true;
    }
}

void addTree(pANTLR3_BASE_TREE tree) {
    if (currentVariables->size == currentVariables->capacity) {
        currentVariables->capacity *= 2;
        currentVariables->trees = realloc(currentVariables->trees,
            sizeof(pANTLR3_BASE_TREE) * currentVariables->capacity);
    }
    currentVariables->trees[currentVariables->size++] = tree;
}

void detectVarDecl(pANTLR3_BASE_TREE tree) {
    if (!tree) return;

    unsigned int childCount = tree->getChildCount(tree);
    for (unsigned int i = 0; i < childCount; ++i) {
        pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)tree->getChild(tree, i);
        if (!child) continue;

        const char* tokenText = (char*)child->getText(child)->chars;

        if (strcmp(tokenText, "VarDecl") == 0 || strcmp(tokenText, "VarDeclToken") == 0) {
            addTree(child);
        }

        detectVarDecl(child);
    }
}

char* getLeftmostLeaf(pANTLR3_BASE_TREE node) {
    if (!node) return NULL;
    if (node->getChildCount(node) == 0) {
        return (char*) node->getText(node)->chars;
    }
    return getLeftmostLeaf((pANTLR3_BASE_TREE)node->getChild(node, 0));
}

void detectTypesOfAllVariables() {
    for (size_t i = 0; i < currentVariables->size; ++i) {
        pANTLR3_BASE_TREE varDecl = currentVariables->trees[i];
        if (!varDecl || varDecl->getChildCount(varDecl) < 2)
            continue;

        pANTLR3_BASE_TREE idNode = varDecl->getChild(varDecl, 0);
        pANTLR3_BASE_TREE exprNode = varDecl->getChild(varDecl, 1);

        if (!idNode || !exprNode)
            continue;

        char* varName = (char*)idNode->getText(idNode)->chars;
        char* representative = getLeftmostLeaf(exprNode);
        dataType inferredType = detectType(currentTable, representative);

        appendSymbolTable(currentTable, strdup(varName), inferredType);
    }

    currentOpPseudoType = UNKNOWN_TYPE;
}

void processTreeFunc(pANTLR3_BASE_TREE tree) {
    tree = tree->getChild(tree, 1);

    currentVariables = malloc(sizeof(listOfVariables));
    currentVariables->size = 0;
    currentVariables->capacity = 4;
    currentVariables->trees = malloc(sizeof(pANTLR3_BASE_TREE) * currentVariables->capacity);

    currentTable = initSymbolTable(list, "temp");
    detectVarDecl(tree);
    detectTypesOfAllVariables();
}

symbolTable* processTreeToBuild(pANTLR3_BASE_TREE tree) {
    if (!tree) return NULL;
    int funcCount = tree->getChildCount(tree);
    list = initListOfTables(funcCount);

    for (int i = 0; i < funcCount; i++) {
        pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)tree->getChild(tree, i);
        processTreeFunc(child);
    }

    return currentTable;
}
