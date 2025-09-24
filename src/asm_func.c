#include "common.h"
#include "cpu.h"
#include "asm_func.h"

void asmNone(CPU * cpu) {
    return;
}

void asmNop(CPU * cpu) {
    printf("Got here\n");
}

void asmLd(CPU * cpu) {
    bool is_16bit = (cpu->instruction->r2 >= R_AF) ? true : false;

    
}

void asmDi(CPU * cpu) {
    cpu->int_enable = false;
}

static ASM_FUNC_PTR asm_functions[I_SET_SIZE] = {
    [I_NONE] = asmNone,
    [I_NOP] = asmNop,
    [I_LD] = asmLd,
    [I_DI] = asmDi
};

ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return asm_functions[i];
}