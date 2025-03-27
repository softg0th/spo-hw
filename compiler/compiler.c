#include "../types/typization.h"

#define asm_code_header "\n[section code_ram]\n\tjump start\n"
#define asm_data_header "[section data_ram]\n_true: db 0x1\n_false: db 0x0\n"
#define asm_footer "halt:\n\thlt\n"
#define label(l) fprintf(file, "%s:\n", l);

#define mnemonic_1(mnemonic, arg1) fprintf(file, "\t%s %s\n", mnemonic, arg1);
#define mnemonic_2(mnemonic, arg1, arg2) fprintf(file, "\t%s %s, %s\n", mnemonic, arg1, arg2);
#define mnemonic_2_i(mnemonic, arg1, arg2) fprintf(file, "\t%s %s, %s\n", mnemonic, arg1, arg2);
#define mnemonic_0(mnemonic) fprintf(file, "\t%s\n", mnemonic);

#define mov_i(from, to) mnemonic_2_i("mov", from, to)
#define push(reg) mnemonic_1("push", reg)
#define pop(reg) mnemonic_1("pop", reg)
#define add(op1, op2) mnemonic_2("add", op1, op2)
#define sub(op1, op2) mnemonic_2("sub", op1, op2)
#define mul(op1, op2) mnemonic_2("mul", op1, op2)
#define div(op1, op2) mnemonic_2("div", op1, op2)
#define rem(op1, op2) mnemonic_2("rem", op1, op2)
#define and(op1, op2) mnemonic_2("_and", op1, op2)
#define or(op1, op2) mnemonic_2("_or", op1, op2)
#define neg(op, to) mnemonic_2("neg", op, to)
#define not(op, to) mnemonic_2("_not", op, to)

#define jeq(op, to) mnemonic_2("jumpeq", op, to)
#define jgt(op, to) mnemonic_2("jumpgt", op, to)
#define jlt(op, to) mnemonic_2("jumplt", op, to)
#define jmp(to) mnemonic_1("jump", to)

#define load(from, to) mnemonic_2("load", from, to)

#define ret() mnemonic_0("ret")
#define call(label) mnemonic_1("call", label)


void compile(pANTLR3_BASE_TREE tree, struct programGraph *graph) {
    symbolTable* symTable = processTreeToBuild(tree);

}