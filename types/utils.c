#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

#include <stdlib.h>

listOfTables* initListOfTables(int funcCount) {
    listOfTables *list = calloc(1, sizeof(listOfTables));
    list->tables = calloc(funcCount + 1, sizeof(symbolTable *));
    return list;
}

symbolTable* initSymbolTable(listOfTables* list, char* name) {
    symbolTable *table = calloc(1, sizeof(symbolTable));

    table->name = strdup(name);
    table->count = 0;
    int i = 0;
    while (list->tables[i] != NULL) {
        i++;
    }
    list->tables[i] = table;

    return table;
}

void appendSymbolTable(symbolTable* table, char* name, dataType type) {
    if (table->count < SYMBOL_TABLE_CAPACITY) {
        table->symbols[table->count].name = strdup(name);
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

dataType detectType(symbolTable* table, char *value) {
    dataType dt = CUSTOM_TYPE;
    if (isInteger(value)) {
        dt = INT_TYPE;
    } else {
        symbol* symb = lookupSymbol(table, value);
        if (symb != NULL) {
            return symb->type;
        }
    }
    return dt;
}

bool isBinop(char *value) {
    const char* binopNames[] = {
        "OP_ADD", "OP_SUB", "OP_MUL", "OP_DIV", "OP_MOD",
        "OP_LSHIFT", "BINOP_RSHIFT", "BINOP_AND", "OP_XOR", "OP_OR",
        "OP_ASSIGN"
    };

    for (int i =0; i < 11; i++) {
        if (strcmp(value, binopNames[i]) == 0) {
            return true;
        }
    }
    return false;
}
