#define SYMBOL_TABLE_CAPACITY 1000

typedef enum {
    UNKNOWN,
    STR,
    CHAR,
    HEX,
    BITS,
    BOOL,
    INT,
    UINT,
    LONG,
    ULONG,
    CUSTOM,
    ARRAY
} dataType;

typedef struct {
    char* name;
    dataType type;
} symbol;

typedef struct {
    symbol symbols[SYMBOL_TABLE_CAPACITY];
    int count;
    char* name;
} symbolTable;

typedef struct {
    struct symbolTable **tables;
} listOfTables;


bool isInteger(char *str);
dataType detectType(char *value);