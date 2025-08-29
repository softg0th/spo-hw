#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <antlr3.h>

struct typeError *errorList = NULL;
listOfTables* list = NULL;
symbolTable* currentTable = NULL;
listOfVariables* currentVariables = NULL;
dataType currentOpPseudoType = UNKNOWN_TYPE;
bool hasErrors = false;


static struct { char* name; const char* reg; } gParamMap[4];
static int gParamCount = 0;

static void setCurrentFunctionParams(const char** names, int count) {
    gParamCount = (count > 4) ? 4 : count;
    for (int i = 0; i < gParamCount; ++i) {
        if (gParamMap[i].name) free(gParamMap[i].name);
        gParamMap[i].name = strdup(names[i]);
        gParamMap[i].reg  = (i==0?"r0": i==1?"r1": i==2?"r2":"r3");
    }
}

static const char* lookupParamReg(const char* ident) {
    for (int i = 0; i < gParamCount; ++i)
        if (strcmp(gParamMap[i].name, ident) == 0) return gParamMap[i].reg;
    return NULL;
}

char* getLeftmostLeaf(pANTLR3_BASE_TREE node) {
    if (!node) return NULL;
    if (node->getChildCount(node) == 0) {
        return (char*) node->getText(node)->chars;
    }
    return getLeftmostLeaf((pANTLR3_BASE_TREE)node->getChild(node, 0));
}

static void collectAndRegisterParams(pANTLR3_BASE_TREE funcNode) {
    if (!funcNode) return;
    pANTLR3_BASE_TREE sig = (pANTLR3_BASE_TREE)funcNode->getChild(funcNode, 0);
    if (!sig) return;

    const char* paramNames[4] = {0};
    int count = 0;

    unsigned int sc = sig->getChildCount(sig);
    for (unsigned int i = 0; i < sc; ++i) {
        pANTLR3_BASE_TREE ch = (pANTLR3_BASE_TREE)sig->getChild(sig, i);
        if (!ch) continue;
        const char* tt = (const char*)ch->getText(ch)->chars;
        if (!tt) continue;

        if (strcmp(tt, "ArgListToken") == 0) {
            unsigned int ac = ch->getChildCount(ch);
            for (unsigned int j = 0; j < ac && count < 4; ++j) {
                pANTLR3_BASE_TREE arg = (pANTLR3_BASE_TREE)ch->getChild(ch, j);
                if (!arg) continue;
                const char* tta = (const char*)arg->getText(arg)->chars;
                if (!tta || strcmp(tta, "ArgToken") != 0) continue;

                pANTLR3_BASE_TREE idTree = (pANTLR3_BASE_TREE)arg->getChild(arg, 0);
                char* nameLeaf = getLeftmostLeaf(idTree);
                if (!nameLeaf) continue;

                dataType dt = UNKNOWN_TYPE;
                if (arg->getChildCount(arg) > 1) {
                    pANTLR3_BASE_TREE typeNode = (pANTLR3_BASE_TREE)arg->getChild(arg, 1);
                    char* typeLeaf = getLeftmostLeaf(typeNode);
                    if (typeLeaf) {
                        dt = detectType(currentTable, typeLeaf);
                    }
                }

                appendSymbolTable(currentTable, strdup(nameLeaf), dt);

                int addr = -1 - count;
                insertSymbolAddress(currentTable, nameLeaf, addr);
                paramNames[count++] = nameLeaf;
            }
            break;
        }
    }
    setCurrentFunctionParams(paramNames, count);
}

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
    // Initialize per-function containers
    currentVariables = malloc(sizeof(listOfVariables));
    currentVariables->size = 0;
    currentVariables->capacity = 4;
    currentVariables->trees = malloc(sizeof(pANTLR3_BASE_TREE) * currentVariables->capacity);

    // Create a fresh symbol table for this function
    currentTable = initSymbolTable(list, "temp");

    // 1) Register parameters (into symbol table and ABI map)
    collectAndRegisterParams(tree);

    // 2) Proceed with body analysis (was previously tree = child(1))
    pANTLR3_BASE_TREE body = (pANTLR3_BASE_TREE)tree->getChild(tree, 1);
    detectVarDecl(body);
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
