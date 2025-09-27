#include "common.h"
#include "gb.h"
#include "cpu.h"
#include "asm_func.h"

static CPU * p_cpu;
/* Consider just passing a pointer to the CPU with an init function. this is stupid. */
static bool asmCheckCondition() {
    bool c_flag = BIT_CHECK(p_cpu->regs.f, CARRY_FLAG);
    bool z_flag = BIT_CHECK(p_cpu->regs.f, ZERO_FLAG);

    switch(p_cpu->instruction->condition) {
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

void asmNone() {
    return;
}

void asmNop() {
    return;
}

void asmLd() {
    bool is_16bit = (p_cpu->instruction->r1 >= R_AF) ? true : false;
    if(p_cpu->to_memory == false) {
        /* Default (easiest option) */
        cpuWriteReg(p_cpu->instruction->r1, p_cpu->data);
    }
}

void asmDi() {
    p_cpu->int_enable = false;
}

void asmJp() {
    /* Lots of cases here... */
    if(asmCheckCondition() == true) {
        p_cpu->regs.pc = p_cpu->data;
        gbTick();
    }
}

void asmInc(void) {

}

void asmDec(void) {

}

void asmRl(void) {

}

void asmRla(void) {

}

void asmJr(void) {

}

void asmAdd(void) {

}

void asmRrca(void) {

}

void asmCpl(void) {

}

void asmCcf(void) {

}

void asmHalt(void) {

}

void asmStop(void) {
    
}

void asmLdh(void) {

}

void asmCp(void) {

}

void asmOr(void) {

}

void asmCall(void) {

}

void asmEi(void) {

}

void asmXor(void) {
    p_cpu->regs.a ^= p_cpu->data & 0xFF;
    if(p_cpu->regs.a == 0x00) {
        cpuSetFlags(1,0,0,0);
    }
}

static ASM_FUNC_PTR asm_functions[I_SET_SIZE] = {
    [I_NONE] = asmNone,
    [I_NOP] = asmNop,
    [I_LD] = asmLd,
    [I_DI] = asmDi,
    [I_JP] = asmJp,
    [I_XOR] = asmXor,
    [I_STOP] = asmStop,
    [I_DEC] = asmDec,
    [I_JR] = asmJr,
    [I_LDH] = asmLdh,
    [I_CP] = asmCp,
    [I_OR] = asmOr,
    [I_INC] = asmInc,
    [I_CALL] = asmCall,
    [I_EI] = asmEi,
    [I_ADD] = asmAdd
};

ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return asm_functions[i];
}

void asmSetCpuPtr(CPU * cpu) {
    p_cpu = cpu;

    printf("\t\t\tcheck: 0x%02X\n", p_cpu->regs.a);
}