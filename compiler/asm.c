#include <stdio.h>

#include "asm.h"

void generateASM(IRInstruction** pool, int count) {
    FILE *asmFile = fopen("out.asm", "w");
    fprintf(asmFile, "[section code_ram]\n");
    fprintf(asmFile, "\tjump start\n\nstart:\n");

    for (int i = 0; i < count; ++i) {
        //printf("%s%s\n", pool[i]->dst, pool[i]->src1);
        switch (pool[i]->op) {
            case IR_MOV:
                printf("mov %s, %s\n", pool[i]->dst, pool[i]->src1);
                fprintf(asmFile, "mov %s, %s\n", pool[i]->dst, pool[i]->src1);
                continue;
            case IR_ADD:
                fprintf(asmFile, "mov %s, %s\n", pool[i]->dst, pool[i]->src1);
                fprintf(asmFile, "add %s, %s\n", pool[i]->dst, pool[i]->src2);
                continue;
            case IR_SUB:
                fprintf(asmFile, "mov %s, %s\n", pool[i]->dst, pool[i]->src1);
                fprintf(asmFile, "sub %s, %s\n", pool[i]->dst, pool[i]->src2);
                continue;
            case IR_MUL:
                fprintf(asmFile, "mov %s, %s\n", pool[i]->dst, pool[i]->src1);
                fprintf(asmFile, "mul %s, %s\n", pool[i]->dst, pool[i]->src2);
                continue;
            case IR_DIV:
                fprintf(asmFile, "mov %s, %s\n", pool[i]->dst, pool[i]->src1);
                fprintf(asmFile, "div %s, %s\n", pool[i]->dst, pool[i]->src2);
                continue;
            case IR_REM:
                continue;
            case IR_NEG:
                continue;
            case IR_NOT:
                continue;
            case IR_AND:
                continue;
            case IR_OR:
                continue;
            case IR_LOAD:
                continue;;
            case IR_STORE:
                continue;
            case IR_JMP:
                continue;
            case IR_JEQ:
                continue;
            case IR_JGT:
                continue;
            case IR_JLT:
                continue;
            case IR_PUSH:
                continue;
            case IR_POP:
                continue;
            case IR_CALL:
                continue;
            case IR_RET:
                continue;
            case IR_LABEL:
                continue;
            default:
                fprintf(stderr, "Unknown IR opcode: %d\n", pool[i]->op);
        }
        i++;
    }
}