#include "../graph/graphForExternal.h"
#include "utils.h"

/*
struct symbol {
    char *identifier;
    enum {
        STR,
        CHAR,
        HEX,
        BIN,
        INT,
        BOOL,
        CUSTOM,
        VOID
    } type;
    bool isParam;
    bool isArray;
    int arrayDimension;
    char *customTypeName;
};

struct symbolTable {
    struct symbol **symbols;
    int numSymbols;
    struct func *f;
};
*/


symbolTable* processGraphToBuild(struct programGraph *graph);