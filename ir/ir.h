typedef enum {
    IR_MOV, IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_REM,
     IR_NEG, IR_NOT, IR_AND, IR_OR,
     IR_LOAD, IR_STORE,
     IR_JMP, IR_JEQ, IR_JGT, IR_JLT,
     IR_PUSH, IR_POP,
     IR_CALL, IR_RET,
     IR_LABEL,
    IR_LOAD_FP,
    IR_STORE_FP,
} IROpcodes;

typedef struct {
    IROpcodes op;
    char* dst;
    char* src1;
    char* src2;
} IRInstruction;

IRInstruction** get_pool();

char* allocLabel();
char* allocLoopLabel(int state);

void emit_add(const char* dst, const char* lhs, const char* rhs);
void emit_mul(const char* dst, const char* lhs, const char* rhs);
void emit_sub(const char* dst, const char* lhs, const char* rhs);
void emit_div(const char* dst, const char* lhs, const char* rhs);
void emit_rem(const char* dst, const char* lhs, const char* rhs);
void emit_mov(const char* dst, const char* src);
void emit_label(char* label);
void emit_jump(char* label);
void emit_cond_jump_false(char* cond, char* label);
void emit_jumpgt(const char* cond, const char* label);
void emit_jumplt(const char* cond, const char* label);
void emit_load(const char* ptr, const char* to);
void emit_store(const char* from, const char* ptr);
void emit_ret();
void emit_call(const char* fname);
void emit_store_fp(char* from, int off);
void emit_load_fp(int off, char* to);