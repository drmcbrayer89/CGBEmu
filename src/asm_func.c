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

void asmJp() {
    /* Lots of cases here... */
    if(asmCheckCondition() == true) {
        p_cpu->regs.pc = p_cpu->data;
        gbTick(1);
    }
}

void asmInc(void) {
    CPU_FLAGS flags = {-1, 0, -1, -1};
    uint16_t val = 0;
    bool is_16bit = (p_cpu->instruction->r1 >= R_AF) ? true : false;

    /* INC r8 */
    if(is_16bit == false) {
        val = (p_cpu->data) + 1;

        if(val == 0x0) {
            flags.z = 1;
        }
        if(val >= 0x10) {
            flags.h = 1;
        }
    }
    /* INC [HL] */
    if(p_cpu->to_memory == true) {
        val = 0;// What do i need to do here?

        if(val == 0x00) {
            flags.z = 1;
        }
        if(val >= 0x10) {
            flags.h = 1;
        }
    }
    
    cpuWriteReg(p_cpu->instruction->r1, val);
    cpuSetFlags(flags);
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
    CPU_FLAGS flags = {-1,0,-1,-1};
    uint16_t val = 0;
    bool is_16bit = (p_cpu->instruction->r2 >= R_AF) ? true: false;

    if(p_cpu->instruction->r1 != R_SP){
        val = cpuReadReg(p_cpu->instruction->r1) + p_cpu->data;

        if(is_16bit == false) {
            if(val == 0x00) {
                flags.z = 1;
            }
            /* These are checking if bits 3 or 7 are exceeded by the sum */
            flags.h = ((val & 0xF)  >= 0x10)  ? true : false;
            flags.c = ((val & 0xFF) >= 0x100) ? true : false;
        }
        /* 16 bit register ...*/
        else {
            /* Checking if bits 11 or 15 are exceeded by the sum */
            uint32_t temp_sum = 0; 
            flags.h = ((val & 0xFFF) >= 0x1000) ? true: false;
            temp_sum = (uint32_t)(cpuReadReg(p_cpu->instruction->r1)) + (uint32_t)p_cpu->data;
            flags.c = (temp_sum >= 0x10000) ? true: false;
            /* Needs more ticks */
            gbTick(1);
        }
    }
    else {
        /* ADD SP, e8 is a special case with signed val (e8) */
        val = cpuReadReg(p_cpu->instruction->r1) + (int8_t)p_cpu->data;
        /* These are checking if bits 3 or 7 are exceeded by the sum */
        flags.z = 0;
        flags.h = ((val & 0xF)  >= 0x10)  ? true : false;
        flags.c = ((val & 0xFF) >= 0x100) ? true : false;
    }

    cpuWriteReg(p_cpu->instruction->r1, val & 0xFFFF);
    cpuSetFlags(flags);
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

void asmCall(void) {

}

void asmEi(void) {
    p_cpu->int_enable = true;
}

void asmDi() {
    p_cpu->int_enable = false;
}

void asmAnd(void) {
    CPU_FLAGS flags = {0,0,0,0};
    p_cpu->regs.a = (p_cpu->regs.a & p_cpu->data);

    if(p_cpu->regs.a == 0x00) {
        flags.z = 1;
    }

    flags.h = 1;
    cpuSetFlags(flags);
}

void asmOr(void) {
    CPU_FLAGS flags = {0,0,0,0};
    p_cpu->regs.a = (p_cpu->regs.a | p_cpu->data);

    if(p_cpu->regs.a == 0x00) {
        flags.z = 1;
    }

    cpuSetFlags(flags);
}

void asmXor(void) {
    CPU_FLAGS flags = {0,0,0,0};
    p_cpu->regs.a ^= p_cpu->data & 0xFF;
    if(p_cpu->regs.a == 0x00) {
        cpuSetFlags(flags);
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
    [I_ADD] = asmAdd,
    [I_AND] = asmAnd
};

ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return asm_functions[i];
}

void asmSetCpuPtr(CPU * cpu) {
    p_cpu = cpu;

    printf("\t\t\tcheck: 0x%02X\n", p_cpu->regs.a);
}