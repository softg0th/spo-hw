#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "utils.h"

#include <stdlib.h>

listOfTables* initListOfTables(int funcCount) {
    struct listOfTables *list = calloc(1, sizeof(struct listOfTables));

    for (unsigned int i = 0; i < funcCount; i++) {
        list->functions = realloc(list->tables,
                sizeof(struct funcNode *) * (funcCount + 1));
    }
    return *list;
}

symbolTable* initSymbolTable(listOfTables* list, char* name) {
    struct symbolTable *table = calloc(1, sizeof(struct symbolTable));

    table->name=name;
    table->count = 0;
    return *table;
}

void appendSymbolTable(symbolTable* table, char* name, dataType type) {
    if (table->count < SYMBOL_TABLE_CAPACITY) {
        strcpy(table->symbols[table->count].name, name);
        table->symbols[table->count].type = type;
        table->count++;
    }
}

symbol* lookupSymbol(symbolTable* table, const char* name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    return NULL;
}

bool isInteger(char *str) {
    if (*str == '\0') return false;

    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

dataType detectType(char *value) {
    dataType dt = CUSTOM;
    if (isInteger(value)) {
        dt = INT;
    }
    return dt;
}
