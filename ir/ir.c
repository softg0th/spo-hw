#include <string.h>
#include <stdlib.h>

#include "ir.h"

#include <stdio.h>

#include "errors.h"

#define IR_CAPACITY 1024

IRInstruction* irPool[IR_CAPACITY];
int irCount = 0;

IRInstruction** get_pool() {
    return irPool;
}

int labelCounter = 0;
int loopCounter = 0;

char* allocLabel() {
    char* buf = malloc(16);
    snprintf(buf, 16, "L%d", labelCounter++);
    return buf;
}

char* allocLoopLabel(int state) {
    char* buf = malloc(16);

    switch (state) {
        case 1:
            snprintf(buf, 16, "Lcond_%d", loopCounter++);
            return buf;
        case 2:
            snprintf(buf, 16, "Lend_%d", loopCounter++);
            return buf;
    }

}

void emit_ir(IROpcodes op, const char* dst, const char* src1, const char* src2) {
    if (irCount >= IR_CAPACITY) {
        handleError(1, NULL);
    }

    IRInstruction* instr = malloc(sizeof(IRInstruction));
    if (!instr) {
        handleError(3, NULL);
    }

    instr->op = op;
    instr->dst = dst ? strdup(dst) : NULL;
    instr->src1 = src1 ? strdup(src1) : NULL;
    instr->src2 = src2 ? strdup(src2) : NULL;

    irPool[irCount++] = instr;
}


void emit_add(const char* dst, const char* lhs, const char* rhs) {
    printf("%s%s%s\n", dst, lhs, rhs);
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
    printf("%s%s\n", dst, src);
    emit_ir(IR_MOV, dst, src, NULL);
}

void emit_label(char* label) {
    emit_ir(IR_LABEL, label, NULL, NULL);
}

void emit_jump(char* label) {
    emit_ir(IR_JMP, label, NULL, NULL);
}

void emit_cond_jump_false(char* cond, char* label) {
    emit_ir(IR_JEQ, cond, "0", label);
}

void emit_jumpgt(const char* cond, const char* label) {
    emit_ir(IR_JGT, cond, label, NULL);
}

void emit_jumplt(const char* cond, const char* label) {
    emit_ir(IR_JLT, cond, label, NULL);
}
