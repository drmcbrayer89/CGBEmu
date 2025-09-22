#include "cpu.h"

void asmNONE(void) {
    return;
}

void asmNOP(void) {
    printf("Got here\n");
}

void asmLD(void) {
    return;
}

static void * asm_functions[I_SET_SIZE] = {
    [I_NONE] = asmNONE,
    [I_NOP]  = asmNOP,
    [I_LD]   = asmLD
};

void * asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return &asm_functions[i];
}