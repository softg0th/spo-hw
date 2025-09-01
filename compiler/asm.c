#include <stdio.h>
#include <string.h>
#include "asm.h"

void generateASM(IRInstruction **pool, int count) {
    FILE *asmFile = fopen("out.asm", "w");
    if (!asmFile) {
        fprintf(stderr, "Failed to open output file\n");
        return;
    }

    const char *entryLabel = NULL;
    for (int i = 0; i < count; ++i) {
        IRInstruction *ins = pool[i];
        if (ins && ins->op == IR_LABEL) {
            if (!entryLabel) entryLabel = ins->dst;
            if (ins->dst && strcmp(ins->dst, "main") == 0) {
                entryLabel = ins->dst;
                break;
            }
        }
    }
    if (!entryLabel) entryLabel = "start";

    fprintf(asmFile, "[section code_ram]\n");
    fprintf(asmFile, "\tjump %s\n\n", entryLabel);

    for (int i = 0; i < count; ++i) {
        IRInstruction *instr = pool[i];
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
                fprintf(asmFile, "neg %s, %s\n", instr->src1, instr->dst);
                break;
            case IR_NOT:
                fprintf(asmFile, "_not %s, %s\n", instr->src1, instr->dst);
                break;
            case IR_AND:
                fprintf(asmFile, "mov %s, %s\n", instr->dst, instr->src1);
                fprintf(asmFile, "_and %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_OR:
                fprintf(asmFile, "mov %s, %s\n", instr->dst, instr->src1);
                fprintf(asmFile, "_or %s, %s\n", instr->dst, instr->src2);
                break;
            case IR_LOAD:
                fprintf(asmFile, "load %s, %s\n", instr->src1, instr->dst);
                break;
            case IR_STORE:
                fprintf(asmFile, "store %s, %s\n", instr->src1, instr->dst);
                break;
            case IR_PUSH:
                fprintf(asmFile, "push %s\n", instr->src1);
                break;
            case IR_POP:
                fprintf(asmFile, "pop %s\n", instr->dst);
                break;
            case IR_CALL:
                fprintf(asmFile, "call %s\n", instr->src1);
                break;
            case IR_RET:
                fprintf(asmFile, "ret\n");
                break;
            default:
                fprintf(stderr, "Unknown IR opcode: %d\n", instr->op);
                break;
        }
    }

    fclose(asmFile);
}
