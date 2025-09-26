#include "common.h"
#include "gb.h"
#include "cpu.h"
#include "asm_func.h"

/* Consider just passing a pointer to the CPU with an init function. this is stupid. */
static bool asmCheckCondition(CPU * cpu) {
    bool c_flag = BIT_CHECK(cpu->regs.f, CARRY_FLAG);
    bool z_flag = BIT_CHECK(cpu->regs.f, ZERO_FLAG);

    switch(cpu->instruction->condition) {
        case C_NONE:
            return true;
        case C_Z:
            return z_flag;
        case C_NZ:
            return !z_flag;
        case C_C:
            return c_flag;
        case C_NC:
            return !c_flag;
        default:
            printf("Wrong condition detected!\n");
            return false;
    }
}

void asmNone(CPU * cpu) {
    return;
}

void asmNop(CPU * cpu) {
    return;
}

void asmLd(CPU * cpu) {
    bool is_16bit = (cpu->instruction->r1 >= R_AF) ? true : false;
    if(cpu->to_memory == false) {
        /* Default (easiest option) */
        cpuWriteReg(cpu->instruction->r1, cpu->data);
    }
}

void asmDi(CPU * cpu) {
    cpu->int_enable = false;
}

void asmJp(CPU * cpu) {
    /* Lots of cases here... */
    if(asmCheckCondition(cpu) == true) {
        cpu->regs.pc = cpu->data;
        gbTick();
    }
}

void asmInc(CPU * cpu) {

}

void asmDec(CPU * cpu) {

}

void asmRl(CPU * cpu) {

}

void asmRla(CPU * cpu) {

}

void asmJr(CPU * cpu) {

}

void asmAdd(CPU * cpu) {

}

void asmRrca(CPU * cpu) {

}

void asmCpl(CPU * cpu) {

}

void asmCcf(CPU * cpu) {

}

void asmHalt(CPU * cpu) {

}

void asmXor(CPU * cpu) {
    cpu->regs.a ^= cpu->data & 0xFF;
    if(cpu->regs.a == 0x00) {
        cpuSetFlags(1,0,0,0);
    } 
}

static ASM_FUNC_PTR asm_functions[I_SET_SIZE] = {
    [I_NONE] = asmNone,
    [I_NOP] = asmNop,
    [I_LD] = asmLd,
    [I_DI] = asmDi,
    [I_JP] = asmJp,
    [I_XOR] = asmXor
};

ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return asm_functions[i];
}