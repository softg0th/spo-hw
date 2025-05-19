#include <stdio.h>

#include "asm.h"

void generateASM(IRInstruction** pool, int count) {
    FILE *asmFile = fopen("out.asm", "w");
    if (!asmFile) {
        fprintf(stderr, "Failed to open output file\n");
        return;
    }

    fprintf(asmFile, "[section code_ram]\n");
    fprintf(asmFile, "\tjump start\n\nstart:\n");

    for (int i = 0; i < count; ++i) {
        IRInstruction* instr = pool[i];
        switch (instr->op) {
            case IR_MOV:
                printf("mov %s, %s\n", instr->dst, instr->src1);
                fprintf(asmFile, "mov %s, %s\n", instr->dst, instr->src1);
                break;
            case IR_ADD:
                fprintf(asmFile, "mov %s, %s\n", instr->dst, instr->src1);
                fprintf(asmFile, "add %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_SUB:
                fprintf(asmFile, "mov %s, %s\n", instr->dst, instr->src1);
                fprintf(asmFile, "sub %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_MUL:
                fprintf(asmFile, "mov %s, %s\n", instr->dst, instr->src1);
                fprintf(asmFile, "mul %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_DIV:
                fprintf(asmFile, "mov %s, %s\n", instr->dst, instr->src1);
                fprintf(asmFile, "div %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_REM:
                fprintf(asmFile, "mov %s, %s\n", instr->dst, instr->src1);
                fprintf(asmFile, "rem %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_JMP:
                fprintf(asmFile, "jump %s\n", instr->dst);
                break;
            case IR_JEQ:
                fprintf(asmFile, "jumpeq %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_JGT:
                fprintf(asmFile, "jumpgt %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_JLT:
                fprintf(asmFile, "jumplt %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_LABEL:
                fprintf(asmFile, "%s:\n", instr->dst);
                break;
            case IR_NEG:
            case IR_NOT:
            case IR_AND:
            case IR_OR:
            case IR_LOAD:
            case IR_STORE:
            case IR_PUSH:
            case IR_POP:
            case IR_CALL:
            case IR_RET:
                break;
            default:
                fprintf(stderr, "Unknown IR opcode: %d\n", instr->op);
                break;
        }
    }

    fclose(asmFile);
}