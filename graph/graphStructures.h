#ifndef GRAPH_STRUCTURES_H
#define GRAPH_STRUCTURES_H

#include <stdbool.h>

struct cfgNode {
    int id;
    char* name;
    char* nodeOp;
    struct astNode* ast;
    struct cfgNode* conditionalBranch;
    struct cfgNode* defaultBranch;
    bool isTraversed;
    bool isProcessed;
    bool isParsed;
    bool isParseTreeRoot;
    int parseTreeHeight;
    struct parseTree* parseTree;
};

struct parseTree {
    char* name;
    struct parseTree* left;
    struct parseTree* right;
};

struct funcNode {
    char *identifier;
    struct astNode *signature;
    struct astNode *body;
    struct cfgNode *cfgEntry;
    struct cfgNode *cfgExit;
    char *sourceFileName;

    int callNum;
    char **callNames;
    struct funcNode **calls;
};

struct context {
    struct cfgNode *curr;
    struct cfgNode *entryNode;
    struct cfgNode *exitNode;

    int loopDepth;
    struct breakStack *breakStack;
    struct funcNode *function;
    struct errorList *errors;
};

struct programGraph {
    struct funcNode **functions;
    int funcCount;
};

struct errorList {
    char *message;
    struct cfgErrorList *next;
};

#endif
