#include "common.h"
#include "gb.h"
#include "cpu.h"
#include "asm_func.h"
#include "bus.h"
#include "stack.h"

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

void gotoAddr(uint16_t addr, bool push) {
    if(asmCheckCondition() == true) {
        if(push == true) {
            stackPush16(p_cpu->regs.pc);
            gbTick(2);
        }

        p_cpu->regs.pc = addr;
        gbTick(1);
    }
}

void asmJp(void) {
    // JP does not push the program counter
    gotoAddr(p_cpu->data, false);
}

void asmJr(void) {
    byte relative_addr = (byte)(p_cpu->data & 0xFF);
    uint16_t new_addr = relative_addr + p_cpu->regs.pc;
    //printf("PC: 0x%04X REL: 0x%04X NEW: 0x%04X BEFORE\n", p_cpu->regs.pc, relative_addr, new_addr);
    gotoAddr(new_addr, false);
    //printf("PC: 0x%04X AFTER\n", p_cpu->regs.pc);
}

void asmCall(void) {
    gotoAddr(p_cpu->data, true);
}

void asmNone(void) {
    return;
}

void asmNop(void) {
    return;
}

void asmLd(void) { 
    bool is_16bit = (p_cpu->instruction->r2 >= R_AF) ? true : false;
    if(p_cpu->to_memory == false) {
        /* Default (easiest option) */
        cpuWriteReg(p_cpu->instruction->r1, p_cpu->data);
        return;
    }
    else if(p_cpu->to_memory == true) {
        if(is_16bit == true) {
            busWriteAddr16(p_cpu->memory_destination, p_cpu->data);
            gbTick(1);
        }
        else{
            busWriteAddr(p_cpu->memory_destination, p_cpu->data);
        }
        gbTick(1);
        return;
    }
    else if(p_cpu->instruction->addr_mode == M_HL_SPR) {
        // Do this later...
    }
}

void asmInc(void) {
    CPU_FLAGS flags = {-1, 0, -1, -1};
    uint16_t val = 0;
    bool is_16bit = (p_cpu->instruction->r1 >= R_AF) ? true : false;

    /* INC r8 */
    if(is_16bit == false) {
        val = p_cpu->data + 1;
        cpuWriteReg(p_cpu->instruction->r1, val & 0xF);

        if(val == 0x0) {
            flags.z = 1;
        }
        if(val >= 0x10) {
            flags.h = 1;
        }
        cpuSetFlags(flags);
    }
    else {
        /* Cycle for extra byte */
        gbTick(1);
        /* INC [HL] */
        if(p_cpu->instruction->r1 == R_HL && p_cpu->to_memory == true) {
            val = busReadAddr(cpuReadReg(R_HL)) + 1;
            val = val & 0xFF;
            busWriteAddr(cpuReadReg(R_HL), val);

            /* 3 total cycles for INC [HL] */
            gbTick(1);

            if(val == 0x00) {
                flags.z = 1;
            }
            if(val >= 0x10) {
                flags.h = 1;
            }
            cpuSetFlags(flags);
        }
        /* INC r16 */
        else {
            val = p_cpu->data + 1;
            val = val & 0xFF;

            cpuWriteReg(p_cpu->instruction->r1, val);
        }
    }
}

void asmDec(void) {
    CPU_FLAGS flags = {-1, 1, -1, -1};
    uint16_t val = 0;
    bool is_16bit = (p_cpu->instruction->r1 >= R_AF) ? true : false;

    /* DEC r8 */
    if(is_16bit == false) {
        val = p_cpu->data - 1;
        cpuWriteReg(p_cpu->instruction->r1, val & 0xF);

        if(val == 0x0) {
            flags.z = 1;
        }
        if(val >= 0x10) {
            flags.h = 1;
        }
        cpuSetFlags(flags);
    }
    else {
        /* Cycle for extra byte */
        gbTick(1);
        /* DEC [HL] */
        if(p_cpu->instruction->r1 == R_HL && p_cpu->to_memory == true) {
            val = busReadAddr(cpuReadReg(R_HL)) - 1;
            val = val & 0xFF;
            busWriteAddr(cpuReadReg(R_HL), val);

            /* 3 total cycles for INC [HL] */
            gbTick(1);

            if(val == 0x00) {
                flags.z = 1;
            }
            if((val & 0x0F) == 0x0F) {
                flags.h = 1;
            }
            cpuSetFlags(flags);
        }
        /* DEC r16 */
        else {
            val = p_cpu->data - 1;
            val = val & 0xFF;

            cpuWriteReg(p_cpu->instruction->r1, val);
        }
    }
}

void asmRla(void) {
    CPU_FLAGS flags = {0,0,0,0};

    uint8_t a_reg = cpuReadReg(R_A);
    uint8_t carry_flag = cpuGetFlag(CARRY_FLAG);
    /* Grab MSB & store as new carry flag */
    uint8_t carry_temp = (a_reg >> 7) & 1;
    /* Shift bits to the left, add carry flag as LSB */
    a_reg = (a_reg << 1) | carry_temp;

    flags.c = carry_temp;
    /* Store rotated register back into A */
    cpuWriteReg(R_A, a_reg);
    cpuSetFlags(flags);
}

void asmRlca(void) {
    CPU_FLAGS flags = {0,0,0,0};

    uint8_t a_reg = cpuReadReg(R_A);
    /* Grab MSB */
    uint8_t msb = (a_reg >> 7) & 1;
    /* Store for later*/
    uint8_t carry_flag = msb;
    /* Shift register to the left, bit 7 added to bit 0 */
    a_reg = (a_reg << 1) | msb;

    flags.c = carry_flag;
    
    cpuWriteReg(R_A, a_reg);
    cpuSetFlags(flags);

    // 1 cycle
    gbTick(1);
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

void asmSub(void) {
    CPU_FLAGS flags = {-1, 1, -1, -1};

    uint8_t result = p_cpu->regs.a - p_cpu->data;

    flags.c = (result == 0) ? 1 : 0;
    flags.h = ((result & 0x0F) == 0x0F) ? 1 : 0;
    flags.c = (p_cpu->data > p_cpu->regs.a) ? 1 : 0;

    cpuWriteReg(p_cpu->instruction->r1, result);
    cpuSetFlags(flags);
}

void asmRrca(void) {
    CPU_FLAGS flags = {0,0,0,0};

    uint8_t a_reg = cpuReadReg(R_A);
    uint8_t lsb = a_reg & 1;
    uint8_t carry_flag = lsb;
    /* Shift A to the right, add LSB to MSB */
    a_reg = (a_reg >> 1) | (lsb << 7);
    flags.c = carry_flag;
    
    cpuWriteReg(R_A, a_reg);
    cpuSetFlags(flags);
}
/* This is just a NOT operator for register A */
void asmCpl(void) {
    CPU_FLAGS flags = {-1, 1, 1, -1};
    p_cpu->regs.a = ~p_cpu->regs.a;
    cpuSetFlags(flags);
}

void asmCcf(void) {
    /* Complement Carry Flag */
    CPU_FLAGS flags = {-1, 0, 0, 0};

    flags.c = !cpuGetFlag(CARRY_FLAG);

    cpuSetFlags(flags);
}

void asmHalt(void) {
    p_cpu->halted = true;
}

void asmStop(void) {
    
}

void asmLdh(void) {
    /* Copy the byte at address $FF00 + C into register A */
    if(p_cpu->instruction->r1 == R_A) {
        cpuWriteReg(R_A, busReadAddr(0xFF00 + p_cpu->data));
    }
    /* Copy value in register A into the byte pointed by R1 */
    else {
        busWriteAddr(p_cpu->memory_destination, p_cpu->regs.a);
    }
}

void asmCp(void) {
    CPU_FLAGS flags = {-1, 1, -1, -1};

    int32_t compare = (int32_t)p_cpu->regs.a - (int32_t)p_cpu->data;

    flags.z = (compare == 0) ? 1 : 0;
    flags.h = ((compare & 0x0F) == 0x0F) ? 1 : 0;
    flags.c = (p_cpu->data > p_cpu->regs.a) ? 1 : 0;

    cpuSetFlags(flags);
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

void asmAdc(void) {
    CPU_FLAGS flags = {-1, 0, -1, cpuGetFlag(CARRY_FLAG)};

    uint16_t a = p_cpu->regs.a;
    uint16_t r8 = p_cpu->data;
    uint16_t val = a + r8 + flags.c;

    p_cpu->regs.a = val & 0xFF;

    flags.z = (val == 0x00) ? 1 : 0;
    flags.h = ((val & 0x0F) > 0x0F) ? 1 : 0;
    flags.c = (val > 0xFF) ? 1 : 0;

    cpuSetFlags(flags);
}

void asmSbc(void) {
    CPU_FLAGS flags = {-1, 1, -1, cpuGetFlag(CARRY_FLAG)};
    uint8_t reg_a = cpuReadReg(R_A);
    if(p_cpu->instruction->r2 == R_HL || p_cpu->instruction->r2 == R_NONE) {
        // SBC A [HL] & SBC A n8 require additional cycles
        gbTick(1);
    }

    // Subtract value in r8/[hl]/n8 and the carry flag from A
    reg_a = reg_a - (flags.c + p_cpu->data);
    cpuWriteReg(R_A, reg_a);

    // Flags...
    flags.z = (reg_a == 0) ? 1 : 0;
    flags.h = ((reg_a & 0xF) < ((p_cpu->data & 0xF) + flags.c)) ? 1 : 0;
    flags.c = ((p_cpu->data + flags.c) > reg_a) ? 1 : 0;
    cpuSetFlags(flags);
}

void asmPop(void) {
    /* These are only used for 16 bit values*/
    /*
    uint16_t lo = stackPop8();
    uint16_t hi = stackPop8();
    uint16_t result = (hi << 8) | lo;
    */
    uint16_t result = stackPop16();
    gbTick(2);
    /* Register AF is a bit different */
    if(p_cpu->instruction->r1 == R_AF) {
        cpuWriteReg(p_cpu->instruction->r1, result & 0xFFF0);
    }
    else {
        cpuWriteReg(p_cpu->instruction->r1, result);
    }
}

void asmPush(void) {
    stackPush16(p_cpu->data);
    gbTick(4);
}

void asmRet(void) {
    /* RET is similar to a POP PC.
       POP value from stack, assign to pc register 
    */
    if(p_cpu->instruction->condition != C_NONE) {
        gbTick(1);
    }

    uint16_t pc = stackPop16();
    p_cpu->regs.pc = pc;
    gbTick(3);
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
    [I_AND] = asmAnd,
    [I_ADC] = asmAdc,
    [I_SUB] = asmSub,
    [I_LDH] = asmLdh,
    [I_RLCA] = asmRlca,
    [I_RRCA] = asmRrca,
    [I_RLA] = asmRla,
    [I_HALT] = asmHalt,
    [I_SBC] = asmSbc,
    [I_POP] = asmPop,
    [I_PUSH] = asmPush,
    [I_RET] = asmRet
};

ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return asm_functions[i];
}

void asmSetCpuPtr(CPU * cpu) {
    p_cpu = cpu;

    printf("\t\t\tcheck: 0x%02X\n", p_cpu->regs.a);
}