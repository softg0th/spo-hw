#include <stdio.h>

#include "../ir/ir.h"

void generateASM(IRInstruction* pool) {
    FILE *asmFile = fopen("out.asm", "w");
    fprintf(asmFile, "[section code_ram]\n");
    fprintf(asmFile, "\tjump start\n\nstart:\n");

    for (int i = 0; i < sizeof(pool); i++) {
        switch (pool[i].op) {
            case IR_ADD:
                fprintf(asmFile, "");
                return;
            case IR_MUL:
                return;
            case IR_SUB:
                return;
            case IR_DIV:
                return;
            case IR_MOV:
                return;
        }
    }
    return;
}