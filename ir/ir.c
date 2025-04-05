#include <string.h>
#include <stdlib.h>

#include "ir.h"
#include "errors.h"

#define IR_CAPACITY 1024

IRInstruction* irPool[IR_CAPACITY];
int irCount = 0;\

IRInstruction** get_pool() {
    return irPool;
}

void emit_ir(IROpcodes op, const char* dst, const char* src1, const char* src2) {
    if (irCount >= IR_CAPACITY) {
        handleError(1);
    }

    IRInstruction* instr = malloc(sizeof(IRInstruction));
    instr->op = op;
    instr->dst = strdup(dst);
    instr->src1 = strdup(src1);
    if (src2 != NULL) {
        instr->src2 = strdup(src2);
    }
    irPool[irCount++] = instr;
}

void emit_add(const char* dst, const char* lhs, const char* rhs) {
    emit_ir(IR_ADD, dst, lhs, rhs);
}

void emit_mul(const char* dst, const char* lhs, const char* rhs) {
    emit_ir(IR_MUL, dst, lhs, rhs);
}

void emit_sub(const char* dst, const char* lhs, const char* rhs) {
    emit_ir(IR_SUB, dst, lhs, rhs);
}

void emit_div(const char* dst, const char* lhs, const char* rhs) {
    emit_ir(IR_DIV, dst, lhs, rhs);
}

void emit_rem(const char* dst, const char* lhs, const char* rhs) {
    emit_ir(IR_REM, dst, lhs, rhs);
}

void emit_mov(const char* dst, const char* src) {
    emit_ir(IR_MOV, dst, src, NULL);
}