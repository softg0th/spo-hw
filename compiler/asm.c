#include <stdio.h>
#include <string.h>
#include "asm.h"

#include <ctype.h>

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

    // Dynamic runtime shims
    fprintf(asmFile,
        "__dyn_add:\n"
        "\tadd r0, r1\n"
        "\tret\n"
        "\n"
        "__dyn_sub:\n"
        "\tsub r0, r1\n"
        "\tret\n"
        "\n"
        "__dyn_mul:\n"
        "\tmul r0, r1\n"
        "\tret\n"
        "\n"
        "__dyn_div:\n"
        "\tdiv r0, r1\n"
        "\tret\n"
        "\n"
        "__dyn_mod:\n"
        "\trem r0, r1\n"
        "\tret\n"
        "\n"
        "__dyn_lt:\n"
        "\tsub r0, r1\n"
        "\tjumplt r0, __dyn_lt_true\n"
        "\tmov 0, r0\n"
        "\tret\n"
        "__dyn_lt_true:\n"
        "\tmov 1, r0\n"
        "\tret\n"
        "\n"
        "__dyn_gt:\n"
        "\tsub r0, r1\n"
        "\tjumpgt r0, __dyn_gt_true\n"
        "\tmov 0, r0\n"
        "\tret\n"
        "__dyn_gt_true:\n"
        "\tmov 1, r0\n"
        "\tret\n"
        "\n"
        "__dyn_eq:\n"
        "\tsub r0, r1\n"
        "\tjumpeq r0, __dyn_eq_true\n"
        "\tmov 0, r0\n"
        "\tret\n"
        "__dyn_eq_true:\n"
        "\tmov 1, r0\n"
        "\tret\n"
        "\n"
        "__dyn_neq:\n"
        "\tsub r0, r1\n"
        "\tjumpeq r0, __dyn_neq_false\n"
        "\tmov 1, r0\n"
        "\tret\n"
        "__dyn_neq_false:\n"
        "\tmov 0, r0\n"
        "\tret\n"
        "\n"
        "__dyn_le:\n"
        "\tsub r0, r1\n"
        "\tjumpgt r0, __dyn_le_false\n"
        "\tmov 1, r0\n"
        "\tret\n"
        "__dyn_le_false:\n"
        "\tmov 0, r0\n"
        "\tret\n"
        "\n"
        "__dyn_ge:\n"
        "\tsub r0, r1\n"
        "\tjumplt r0, __dyn_ge_false\n"
        "\tmov 1, r0\n"
        "\tret\n"
        "__dyn_ge_false:\n"
        "\tmov 0, r0\n"
        "\tret\n"
        "\n"
        "__dyn_truthy:\n"
        "\tmov 0, r2\n"
        "\tsub r0, r2\n"
        "\tjumpeq r0, __dyn_truthy_false\n"
        "\tmov 1, r0\n"
        "\tret\n"
        "__dyn_truthy_false:\n"
        "\tmov 0, r0\n"
        "\tret\n"
        "\n"
        "__io_out:\n"
        "\tmov r0, outReg\n"
        "\tret\n"
        "\n"
        "__io_in:\n"
        "\t; returns 0 for now (stub)\n"
        "\tmov 0, r0\n"
        "\tret\n"
        "\n"
        "__io_println:\n"
        "\tmov 10, outReg\n"
        "\tret\n"
        "\n"
        "__io_print_str:\n"
        "\t; expects r0 = address of zero-terminated string (ignored in stub)\n"
        "\tret\n"
        "\n"
    );

    for (int i = 0; i < count; ++i) {
        IRInstruction *instr = pool[i];
        switch (instr->op) {
            case IR_MOV: {
                const char* dst = instr->dst;   // куда пишем
                const char* src = instr->src1;  // что пишем

                // простой тест на immediate: десятичное/hex/битовое/символьное
                int is_imm = 0;
                if (src && (
                    isdigit((unsigned char)src[0]) ||
                    (src[0]=='0' && (src[1]=='x'||src[1]=='X')) ||
                    (src[0]=='0' && (src[1]=='b'||src[1]=='B')) ||
                    (src[0]=='\'' && src[strlen(src)-1]=='\'')
                )) is_imm = 1;

                if (is_imm) {
                    // mov-imm2reg (value, to)
                    fprintf(asmFile, "mov %s, %s\n", src, dst);
                } else {
                    // mov-reg2reg (from, to)
                    fprintf(asmFile, "mov %s, %s\n", src, dst);
                }
                break;
            }
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
            case IR_LOAD_FP:
                fprintf(asmFile, "loadfp %s, %s\n", instr->src1, instr->dst);
                break;
            case IR_STORE_FP:
                fprintf(asmFile, "storefp %s, %s\n", instr->src1, instr->dst);
                break;
            default:
                fprintf(stderr, "Unknown IR opcode: %d\n", instr->op);
                break;
        }
    }

    fclose(asmFile);
}
