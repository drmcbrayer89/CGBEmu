#include "cpu.h"
#include "asm_func.h"

void asmNONE(void) {
    return;
}

void asmNOP(void) {
    printf("Got here\n");
}

void asmLD(void) {
    return;
}

static ASM_FUNC_PTR asm_functions[I_SET_SIZE] = {
    [I_NONE] = asmNONE,
    [I_NOP]  = asmNOP,
    [I_LD]   = asmLD
};

ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return asm_functions[i];
}