typedef enum {
    IR_MOV, IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_REM,
     IR_NEG, IR_NOT, IR_AND, IR_OR,
     IR_LOAD, IR_STORE,
     IR_JMP, IR_JEQ, IR_JGT, IR_JLT,
     IR_PUSH, IR_POP,
     IR_CALL, IR_RET,
     IR_LABEL
} IROpcodes;

typedef struct {
    IROpcodes op;
    char* dst;
    char* src1;
    char* src2;
} IRInstruction;

IRInstruction** get_pool();

void emit_add(const char* dst, const char* lhs, const char* rhs);
void emit_mul(const char* dst, const char* lhs, const char* rhs);
void emit_sub(const char* dst, const char* lhs, const char* rhs);
void emit_div(const char* dst, const char* lhs, const char* rhs);
void emit_rem(const char* dst, const char* lhs, const char* rhs);
void emit_mov(const char* dst, const char* src);