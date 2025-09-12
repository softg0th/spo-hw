#include <stdio.h>
#include <string.h>
#include "asm.h"

#include <ctype.h>

static const char* io_alias(const char* s) {
    if (!s) return s;
    if (strcmp(s, "inReg") == 0 || strcmp(s, "inp") == 0)   return "inp";
    if (strcmp(s, "outReg") == 0 || strcmp(s, "outp") == 0) return "outp";
    return s;
}

static const char* io_fix(const char* s) {
    if (!s) return NULL;
    if (strcmp(s, "inp")  == 0) return "inReg";
    if (strcmp(s, "outp") == 0) return "outReg";
    return s;
}

void generateASM(IRInstruction **pool, int count) {
    FILE *asmFile = fopen("works.asm", "w");
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

    fprintf(asmFile,
            "[section code_ram]\n"
            "\tmov 0xFFFE, sp\n"
            "\tmov 0, fp\n"
            "\tcall __bootstrap\n"
            "\thlt\n\n");

    fprintf(asmFile,
    "__bootstrap:\n"
    "\t; read operator char -> 4104\n"
    "\tcall __read_op\n"
    "\tstore r0, 4104\n"
    "\t; read first number -> 4106\n"
    "\tcall __read_u16\n"
    "\tstore r0, 4106\n"
    "\t; read second number -> 4108\n"
    "\tcall __read_u16\n"
    "\tstore r0, 4108\n"
    "\t; jump to user's entry\n"
    "\tcall %s\n"
    "\tret\n\n",
    entryLabel
);
    /*
    fprintf(asmFile,
        "__bootstrap:\n"
        "\tcall __read_u16\n"
        "\tstore r0, 4104\n"
        "\tcall __read_op\n"
        "\tstore r0, 4102\n"
        "\tcall __read_u16\n"
        "\tstore r0, 4106\n"
        "\tcall %s\n"
        "\tret\n\n",
        entryLabel
    );
    */
    // Dynamic runtime shims
    /*
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
    "\t; read a byte/word from input storage\n"
    "\tmov inReg, r0\n"
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
    "__print_u16:\n"
    "\t; prints unsigned integer from r0 in decimal to outReg\n"
    "\t; handle zero fast\n"
    "\tmov r0, r1\n"
    "\tjumpeq r1, __print_u16_zero\n"
    "\tmov 0, r3        ; digit count\n"
    "__print_u16_loop:\n"
    "\tmov r0, r1       ; r1 = value\n"
    "\tmov 10, r2\n"
    "\trem r1, r2       ; r1 = value % 10\n"
    "\tpush r1          ; push digit\n"
    "\tmov r3, r1\n"
    "\tmov 1, r2\n"
    "\tadd r3, r2       ; count++\n"
    "\tmov r0, r1\n"
    "\tmov 10, r2\n"
    "\tdiv r0, r2       ; r0 = value / 10\n"
    "\tjumpeq r0, __print_u16_emit\n"
    "\tjump __print_u16_loop\n"
    "__print_u16_emit:\n"
    "__print_u16_emit_loop:\n"
    "\tmov r3, r1\n"
    "\tjumpeq r1, __print_u16_done ; while (count!=0)\n"
    "\tpop r1\n"
    "\tmov 48, r2\n"
    "\tadd r1, r2       ; '0' + digit\n"
    "\tmov r1, outReg\n"
    "\tmov r3, r1\n"
    "\tmov 1, r2\n"
    "\tsub r1, r2       ; count--\n"
    "\tmov r1, r3\n"
    "\tjump __print_u16_emit_loop\n"
    "__print_u16_zero:\n"
    "\tmov 48, outReg\n"
    "\tret\n"
    "__print_u16_done:\n"
    "\tret\n"
    "\n"
    "__read_u16:\n"
    "\tmov 0, r0\n"
    "__ru_loop:\n"
    "\tmov inReg, r1\n"
    "\tmov r1, r2\n"
    "\tmov 10, r3\n"
    "\tsub r2, r3\n"
    "\tjumpeq r2, __ru_done\n"
    "\tmov 48, r2\n"
    "\tsub r1, r2\n"
    "\tmov r0, r2\n"
    "\tmov 10, r3\n"
    "\tmul r2, r3\n"
    "\tadd r2, r1\n"
    "\tmov r2, r0\n"
    "\tjump __ru_loop\n"
    "__ru_done:\n"
    "\tret\n"
    "\n"
    "__read_op:\n"
    "__ro_loop:\n"
    "\tmov inReg, r0\n"
    "\tmov r0, r1\n"
    "\tmov 10, r2\n"
    "\tsub r1, r2\n"
    "\tjumpeq r1, __ro_loop\n"
    "\tmov r0, r1\n"
    "\tmov 13, r2\n"
    "\tsub r1, r2\n"
    "\tjumpeq r1, __ro_loop\n"
    "\tret\n"
    "\n"
);
*/
    fprintf(asmFile,
        "; ===== print unsigned 16 in r0 =====\n"
        "__print_u16:\n"
        "    push r1\n"
        "    push r2\n"
        "    push r3\n"
        "    mov r0, r1\n"
        "    jumpeq r1, __prz\n"
        "    mov 0, r3\n"
        "__ploop:\n"
        "    mov r0, r1\n"
        "    mov 10, r2\n"
        "    rem r1, r2\n"
        "    push r1\n"
        "    mov r3, r2\n"
        "    mov 1, r1\n"
        "    add r2, r1\n"
        "    mov r2, r3\n"
        "    mov r0, r1\n"
        "    mov 10, r2\n"
        "    div r1, r2\n"
        "    mov r1, r0\n"
        "    jumpeq r0, __pemit\n"
        "    jump __ploop\n"
        "__pemit:\n"
        "__pemit_loop:\n"
        "    mov r3, r1\n"
        "    jumpeq r1, __pdone\n"
        "    pop r1\n"
        "    mov 48, r2\n"
        "    add r1, r2\n"
        "    mov r1, outReg\n"
        "    mov r3, r2\n"
        "    mov 1, r1\n"
        "    sub r2, r1\n"
        "    mov r2, r3\n"
        "    jump __pemit_loop\n"
        "__prz:\n"
        "    mov 48, outReg\n"
        "__pdone:\n"
        "    pop r3\n"
        "    pop r2\n"
        "    pop r1\n"
        "    ret\n\n"

        "; ===== read unsigned 16 from stdin to r0 =====\n"
        "__read_u16:\n"
        "    mov 0, r0\n"
        "__ru_loop:\n"
        "    mov inReg, r1\n"
        "    mov r1, r2\n"
        "    mov 10, r3\n"
        "    sub r2, r3\n"
        "    jumpeq r2, __ru_done\n"
        "    mov r1, r2\n"
        "    mov 13, r3\n"
        "    sub r2, r3\n"
        "    jumpeq r2, __ru_loop\n"
        "    mov r1, r2\n"
        "    mov 48, r3\n"
        "    sub r2, r3\n"
        "    jumplt r2, __ru_loop\n"
        "    mov r2, r1\n"
        "    mov 10, r3\n"
        "    sub r1, r3\n"
        "    jumplt r1, __ru_digit\n"
        "    jump __ru_loop\n"
        "__ru_digit:\n"
        "    mul r0, 10\n"
        "    add r0, r2\n"
        "    jump __ru_loop\n"
        "__ru_done:\n"
        "    ret\n\n"

        "; ===== read operator char to r0 =====\n"
        "__read_op:\n"
        "__ro_loop:\n"
        "    mov inReg, r0\n"
        "    mov r0, r1\n"
        "    mov 10, r2\n"
        "    sub r1, r2\n"
        "    jumpeq r1, __ro_loop\n"
        "    mov r0, r1\n"
        "    mov 13, r2\n"
        "    sub r1, r2\n"
        "    jumpeq r1, __ro_loop\n"
        "    ret\n\n"

        "__io_println:\n"
        "    mov 10, outReg\n"
        "    ret\n\n"
    );

    for (int i = 0; i < count; ++i) {
        IRInstruction *instr = pool[i];
        switch (instr->op) {
            case IR_MOV: {
                const char* dst0 = io_fix(instr->dst);
                const char* src0 = io_fix(instr->src1);

                if (dst0 && src0 && strcmp(dst0, src0) == 0) {
                    // mov rX, rX — пропускаем
                    break;
                }

                /*
                // 1) Блокируем «сырые» чтения stdin: mov inReg/inp, rX → чтобы не висло
                if (src0 && (!strcmp(src0, "inReg") || !strcmp(src0, "inp"))) {
                    // оставим комментарий и положим 0, чтобы не ломать последующие store
                    fprintf(asmFile, "; dropped blocking read: mov %s, %s\n", src0, dst0);
                    fprintf(asmFile, "mov 0, %s\n", dst0);
                    break;
                }
                */
                // 2) Иммедиаты + поддержка одной ASCII-кавычки: "+", "-", "\n" и т.п.
                int is_imm = 0;
                if (src0 && (
                    isdigit((unsigned char)src0[0]) ||
                    (src0[0]=='0' && (src0[1]=='x'||src0[1]=='X')) ||
                    (src0[0]=='0' && (src0[1]=='b'||src0[1]=='B')) ||
                    (src0[0]=='\'' && src0[strlen(src0)-1]=='\'')
                )) {
                    is_imm = 1;
                }

                if (!is_imm) {
                    size_t slen = src0 ? strlen(src0) : 0;
                    if (slen >= 2 && src0[0]=='"' && src0[slen-1]=='"') {
                        int v = 0;
                        if (slen == 3) {
                            v = (unsigned char)src0[1];
                        } else if (slen == 4 && src0[1]=='\\') {
                            char c = src0[2];
                            v = (c=='n')?10 : (c=='r')?13 : (c=='t')?9 : (unsigned char)c;
                        } else {
                            v = (unsigned char)src0[1];
                        }
                        char imm_ascii_buf[16];
                        snprintf(imm_ascii_buf, sizeof(imm_ascii_buf), "%d", v);
                        src0 = imm_ascii_buf;
                        is_imm = 1;
                    }
                }

                // 3) Обычный mov (рег-рег или imm-рег) — синтаксис одинаковый
                fprintf(asmFile, "mov %s, %s\n", src0, dst0);
                break;
            }
            case IR_ADD: {
    const char* d = io_fix(instr->dst);
    const char* a = io_fix(instr->src1);
    const char* b = io_fix(instr->src2);
    if (strcmp(d, a) != 0) {
        // ВАЖНО: mov src, dst  (а не dst, src!)
        fprintf(asmFile, "mov %s, %s\n", a, d);
    }
    // простая проверка иммедиата (включая знак и 0x/0b)
    int isimm = (b && (isdigit((unsigned char)b[0]) || b[0]=='-' ||
                      (b[0]=='0' && (b[1]=='x'||b[1]=='X'||b[1]=='b'||b[1]=='B'))));
    if (isimm) {
        fprintf(asmFile, "mov %s, r2\n", b);
        fprintf(asmFile, "add %s, r2\n", d);
    } else {
        fprintf(asmFile, "add %s, %s\n", d, b);
    }
    break;
}
case IR_SUB: {
    const char* d = io_fix(instr->dst);
    const char* a = io_fix(instr->src1);
    const char* b = io_fix(instr->src2);
    if (strcmp(d, a) != 0) {
        fprintf(asmFile, "mov %s, %s\n", a, d);
    }
    int isimm = (b && (isdigit((unsigned char)b[0]) || b[0]=='-' ||
                      (b[0]=='0' && (b[1]=='x'||b[1]=='X'||b[1]=='b'||b[1]=='B'))));
    if (isimm) {
        fprintf(asmFile, "mov %s, r2\n", b);
        fprintf(asmFile, "sub %s, r2\n", d);
    } else {
        fprintf(asmFile, "sub %s, %s\n", d, b);
    }
    break;
}
case IR_MUL: {
    const char* d = io_fix(instr->dst);
    const char* a = io_fix(instr->src1);
    const char* b = io_fix(instr->src2);
    if (strcmp(d, a) != 0) {
        fprintf(asmFile, "mov %s, %s\n", a, d);
    }
    int isimm = (b && (isdigit((unsigned char)b[0]) || b[0]=='-' ||
                      (b[0]=='0' && (b[1]=='x'||b[1]=='X'||b[1]=='b'||b[1]=='B'))));
    if (isimm) {
        fprintf(asmFile, "mov %s, r2\n", b);
        fprintf(asmFile, "mul %s, r2\n", d);
    } else {
        fprintf(asmFile, "mul %s, %s\n", d, b);
    }
    break;
}
case IR_DIV: {
    const char* d = io_fix(instr->dst);
    const char* a = io_fix(instr->src1);
    const char* b = io_fix(instr->src2);
    if (strcmp(d, a) != 0) {
        fprintf(asmFile, "mov %s, %s\n", a, d);
    }
    int isimm = (b && (isdigit((unsigned char)b[0]) || b[0]=='-' ||
                      (b[0]=='0' && (b[1]=='x'||b[1]=='X'||b[1]=='b'||b[1]=='B'))));
    if (isimm) {
        fprintf(asmFile, "mov %s, r2\n", b);
        fprintf(asmFile, "div %s, r2\n", d);
    } else {
        fprintf(asmFile, "div %s, %s\n", d, b);
    }
    break;
}
case IR_REM: {
    const char* d = io_fix(instr->dst);
    const char* a = io_fix(instr->src1);
    const char* b = io_fix(instr->src2);
    if (strcmp(d, a) != 0) {
        fprintf(asmFile, "mov %s, %s\n", a, d);
    }
    // rem допускаем только рег-рег (как у тебя было), при иммедиате — через r2
    int isimm = (b && (isdigit((unsigned char)b[0]) || b[0]=='-' ||
                      (b[0]=='0' && (b[1]=='x'||b[1]=='X'||b[1]=='b'||b[1]=='B'))));
    if (isimm) {
        fprintf(asmFile, "mov %s, r2\n", b);
        fprintf(asmFile, "rem %s, r2\n", d);
    } else {
        fprintf(asmFile, "rem %s, %s\n", d, b);
    }
    break;
}
            case IR_JMP:
                fprintf(asmFile, "jump %s\n", instr->dst);
                break;
            case IR_JEQ:
                fprintf(asmFile, "jumpeq %s, %s\n", io_fix(instr->src1), instr->dst);
                break;
            case IR_JGT:
                fprintf(asmFile, "jumpgt %s, %s\n", io_fix(instr->src1), instr->dst);
                break;
            case IR_JLT:
                fprintf(asmFile, "jumplt %s, %s\n", io_fix(instr->src1), instr->dst);
                break;
            case IR_JNE:
                fprintf(asmFile, "jumpne %s, %s\n", io_fix(instr->src1), instr->dst);
                break;
            case IR_LABEL:
                fprintf(asmFile, "%s:\n", instr->dst);
                break;
            case IR_NEG:
                fprintf(asmFile, "neg %s, %s\n", io_fix(instr->src1), io_fix(instr->dst));
                break;
            case IR_NOT:
                fprintf(asmFile, "_not %s, %s\n", io_fix(instr->src1), io_fix(instr->dst));
                break;
            case IR_AND: {
                const char* d = io_fix(instr->dst);
                const char* a = io_fix(instr->src1);
                const char* b = io_fix(instr->src2);
                if (strcmp(d, a) != 0) fprintf(asmFile, "mov %s, %s\n", a, d);
                fprintf(asmFile, "_and %s, %s\n", d, b);
                break;
            }
            case IR_OR: {
                const char* d = io_fix(instr->dst);
                const char* a = io_fix(instr->src1);
                const char* b = io_fix(instr->src2);
                if (strcmp(d, a) != 0) fprintf(asmFile, "mov %s, %s\n", a, d);
                fprintf(asmFile, "_or %s, %s\n", d, b);
                break;
            }
            case IR_LOAD:
                fprintf(asmFile, "load %s, %s\n", instr->src1, io_fix(instr->dst));
                break;
            case IR_STORE:
                fprintf(asmFile, "store %s, %s\n", io_fix(instr->src1), instr->dst);
                break;
            case IR_PUSH:
                fprintf(asmFile, "push %s\n", io_fix(instr->src1));
                break;
            case IR_POP:
                fprintf(asmFile, "pop %s\n", io_fix(instr->dst));
                break;
            case IR_CALL:
                fprintf(asmFile, "call %s\n", instr->src1);
                break;
            case IR_RET:
                fprintf(asmFile, "ret\n");
                break;
            case IR_LOAD_FP:
                fprintf(asmFile, "loadfp %s, %s\n", instr->src1, io_fix(instr->dst));
                break;
            case IR_STORE_FP:
                fprintf(asmFile, "storefp %s, %s\n", io_fix(instr->src1), instr->dst);
                break;
            default:
                fprintf(stderr, "Unknown IR opcode: %d\n", instr->op);
                break;
        }
    }

    fclose(asmFile);
}
