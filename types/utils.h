#include <stdbool.h>
#include <antlr3.h>

#define SYMBOL_TABLE_CAPACITY 1000

typedef enum {
    UNKNOWN_TYPE,
    STR_TYPE,
    CHAR_TYPE,
    HEX_TYPE,
    BITS_TYPE,
    BOOL_TYPE,
    INT_TYPE,
    UINT_TYPE,
    LONG_TYPE,
    ULONG_TYPE,
    CUSTOM_TYPE,
    ARRAY_TYPE
} dataType;

typedef struct {
    char* name;
    dataType type;
    int address;
} symbol;

typedef struct {
    symbol symbols[SYMBOL_TABLE_CAPACITY];
    int count;
    char* name;
} symbolTable;

typedef struct {
    struct symbolTable **tables;
} listOfTables;

typedef struct {
    pANTLR3_BASE_TREE* trees;
    size_t size;
    size_t capacity;
} listOfVariables;


bool isInteger(char *str);
dataType detectType(symbolTable* table, char *value);

struct typeError {
    char* value;
    dataType expectedType;
    struct typeError *next;
};

void addTypeError(struct typeError **elist, char* value, dataType expectedType);
void printErrors(struct typeError *elist);
listOfTables* initListOfTables(int funcCount);
symbolTable* initSymbolTable(listOfTables* list, char* name);
void appendSymbolTable(symbolTable* table, char* name, dataType type);
bool insertSymbolAddress(symbolTable* table, const char* name, int address);