#include "common.h"
#include "gb.h"
#include "asm_func.h"
#include "bus.h"
#include "stack.h"

static CPU * p_cpu;
/* Consider just passing a pointer to the CPU with an init function. this is stupid. */
static bool asmCheckCondition(CPU * p_cpu) {
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

void gotoAddr(CPU * p_cpu, uint16_t addr, bool push) {
    if(asmCheckCondition(p_cpu) == true) {
        if(push == true) {
            stackPush16(p_cpu->regs.pc);
            gbTick(2);
        }

        p_cpu->regs.pc = addr;
        gbTick(1);
    }
}

void asmJp(CPU * p_cpu) {
    // JP does not push the program counter
    gotoAddr(p_cpu, p_cpu->data, false);
}

void asmJr(CPU * p_cpu) {
    int8_t relative_addr = (byte)(p_cpu->data & 0xFF);
    uint16_t new_addr = relative_addr + p_cpu->regs.pc;
    gotoAddr(p_cpu, new_addr, false);
}

void asmCall(CPU * p_cpu) {
    gotoAddr(p_cpu, p_cpu->data, true);
}

void asmRst(CPU * p_cpu) {
    stackPush16(p_cpu->regs.pc + 1);
    gbTick(2);

    p_cpu->regs.pc = p_cpu->instruction->parameter;
}

void asmNone(CPU * p_cpu) {
    return;
}

void asmNop(CPU * p_cpu) {
    return;
}

void asmLd(CPU * p_cpu) { 
    bool is_16bit = (p_cpu->instruction->r2 >= R_AF) ? true : false;
    if(p_cpu->to_memory) {
        if(is_16bit) {
            gbTick(1);
            busWriteAddr16(p_cpu->memory_destination, p_cpu->data);
        }
        else {
            busWriteAddr(p_cpu->memory_destination, p_cpu->data);
        }
        gbTick(1);

        return;
    }

    /* Special case to load stack pointer & e8 offset into HL */
    if(p_cpu->instruction->addr_mode == M_HL_SPR) {
        CPU_FLAGS flags = {-1,-1,-1,-1};
        uint16_t sp = cpuReadReg(p_cpu->instruction->r2);
        uint8_t h_flag = ((sp & 0xF) + (p_cpu->data & 0xF)) > 0x0F;
        uint8_t c_flag = ((sp & 0xFF) + (p_cpu->data &0xFF)) > 0xFF;
        
        // z n h c
        flags.z = 0;
        flags.n = 0;
        flags.h = h_flag;
        flags.c = c_flag;
        cpuSetFlags(flags);
    
        // write stack ptr + e8 (signed int8) to r1
        cpuWriteReg(p_cpu->instruction->r1, sp + (uint8_t)p_cpu->data);
        return;
    }
    cpuWriteReg(p_cpu->instruction->r1, p_cpu->data);
}

void asmInc(CPU * p_cpu) {
    //uint16_t val;
    bool is_16bit = (p_cpu->instruction->r1 >= R_AF) ? 1 : 0;
    
    /* INC r8 */
    if(!is_16bit) {
        uint8_t val8 = cpuReadReg(p_cpu->instruction->r1);
        uint8_t val8_inc = val8 + 1;
        cpuWriteReg(p_cpu->instruction->r1, val8_inc);

        CPU_FLAGS flags = {-1,-1,-1,-1};
        flags.z = (val8_inc == 0x00) ? 1 : 0;
        flags.n = 0;
        flags.h = ((val8_inc & 0x0F) == 0) ? 1 : 0;
        flags.c = -1;
        cpuSetFlags(flags);
        
        return;
    }
    /* INC [HL] */
    if(p_cpu->instruction->r1 == R_HL && p_cpu->instruction->addr_mode == M_MEMREG) {
        gbTick(1);
        uint16_t hl_addr = cpuReadReg(R_HL);
        uint8_t val8 = busReadAddr(hl_addr);
        uint8_t val8_inc = val8 + 1;
        busWriteAddr(hl_addr, val8_inc);
        
        CPU_FLAGS flags = {-1,-1,-1,-1};
        flags.z = (val8_inc == 0) ? 1 : 0;
        flags.n = 0;
        flags.h = ((val8_inc & 0x0F) == 0) ? 1 : 0;
        flags.c = -1;
        cpuSetFlags(flags);
        
        return;
    }

    /* INC r16 / sp */
    if(is_16bit) {
        gbTick(1);
        uint16_t val16 = cpuReadReg(p_cpu->instruction->r1);
        uint16_t val16_inc = val16 + 1;
        cpuWriteReg(p_cpu->instruction->r1, val16_inc);
        return;
    }
}

void asmDec(CPU * p_cpu) {
    bool is_16bit = (p_cpu->instruction->r1 >= R_AF) ? true : false;
    if(is_16bit) gbTick(1);

    /* DEC r8 */
    if(!is_16bit) {
        uint8_t val8 = cpuReadReg(p_cpu->instruction->r1);
        uint8_t val8_dec =val8 - 1;
        cpuWriteReg(p_cpu->instruction->r1, val8_dec);

        CPU_FLAGS flags = {-1, -1, -1, -1};
        flags.z = (val8_dec == 0x00) ? 1 : 0;
        flags.n = 1;
        flags.h = ((val8_dec & 0x0F) == 0x0F) ? 1 : 0;
        flags.c = -1;
        cpuSetFlags(flags);
        
        return;
    }

    /* DEC [HL] */
    if(p_cpu->instruction->r1 == R_HL && p_cpu->instruction->addr_mode == M_MEMREG) {
        uint16_t hl_addr = cpuReadReg(R_HL);
        uint8_t val8 = busReadAddr(hl_addr);
        uint8_t val8_dec = val8 - 1;
        busWriteAddr(hl_addr, val8_dec);

        CPU_FLAGS flags = {-1, -1, -1, -1};
        flags.z = (val8_dec == 0) ? 1 : 0;
        flags.n = 1;
        flags.h = ((val8_dec & 0x0F) == 0x0F) ? 1 : 0;
        flags.c = -1;
        cpuSetFlags(flags);

        return;
    }

    /* DEC r16 / DEC sp*/
    if(is_16bit) {
        uint16_t val16 = cpuReadReg(p_cpu->instruction->r1);
        uint16_t val16_dec = val16 - 1;
        cpuWriteReg(p_cpu->instruction->r1, val16_dec);
        return;
    }

}

void asmRla(CPU * p_cpu) {
    CPU_FLAGS flags = {0,0,0,0};

    uint8_t a_reg = cpuReadReg(R_A);
    uint8_t carry_flag = cpuGetFlag(CARRY_FLAG);
    /* Grab MSB & store as new carry flag */
    uint8_t carry_temp = (a_reg >> 7) & 1;
    /* Shift bits to the left, add carry flag as LSB */
    a_reg = (a_reg << 1) | carry_flag;

    flags.c = carry_temp;
    /* Store rotated register back into A */
    cpuWriteReg(R_A, a_reg);
    cpuSetFlags(flags);
}

void asmRlca(CPU * p_cpu) {
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

void asmAdd(CPU * p_cpu) {
    CPU_FLAGS flags = {-1,0,-1,-1};
    uint16_t val = 0;
    bool is_16bit = (p_cpu->instruction->r1 >= R_AF) ? true: false;

    if(p_cpu->instruction->r1 != R_SP){
        val = cpuReadReg(p_cpu->instruction->r1);

        if(is_16bit == false) {
            if(((val + p_cpu->data) & 0xFF) == 0x00) {
                flags.z = 1;
            }
            /* These are checking if bits 3 or 7 are exceeded by the sum */
            flags.h = ((val & 0xF) + (p_cpu->data & 0xF)) > 0xF ? true : false;
            flags.c = ((val + p_cpu->data) > 0xFF) ? true : false;
            cpuWriteReg(p_cpu->instruction->r1, (val + p_cpu->data) & 0xFF);
        }
        /* 16 bit register ...*/
        else {
            /* Checking if bits 11 or 15 are exceeded by the sum */
            uint32_t tmp = (uint32_t)val + (uint32_t)p_cpu->data;
            flags.h = ((val & 0x0FFF) + (p_cpu->data & 0x0FFF)) > 0x0FFF ? 1 : 0;
            flags.c = (tmp > 0xFFFF) ? 1 : 0;

            cpuWriteReg(p_cpu->instruction->r1, tmp & 0xFFFF);
            /* Needs more ticks */
            gbTick(1);
        }
    }
    else {
        /* ADD SP, e8 is a special case with signed val (e8) */
        uint16_t sp = cpuReadReg(p_cpu->instruction->r1);
        int8_t signed_data = (int8_t)p_cpu->data;
        uint16_t sp_new = sp + signed_data;
        /* These are checking if bits 3 or 7 are exceeded by the sum */
        flags.z = 0;
        flags.h = ((sp & 0x0F) + (signed_data & 0x0F)) > 0x0F ? 1 : 0;
        flags.c = ((sp & 0xFF) + (signed_data & 0xFF)) > 0xFF ? 1 : 0;

        cpuWriteReg(p_cpu->instruction->r1, sp_new);
    }

    //cpuWriteReg(p_cpu->instruction->r1, val & 0xFFFF);
    cpuSetFlags(flags);
}

void asmSub(CPU * p_cpu) {
    CPU_FLAGS flags = {-1, 1, -1, -1};

    uint8_t result = p_cpu->regs.a - p_cpu->data;

    flags.z = (result == 0) ? 1 : 0;
    flags.h = ((p_cpu->regs.a & 0x0F) < (p_cpu->data & 0x0F)) ? 1 : 0;
    flags.c = (p_cpu->data > p_cpu->regs.a) ? 1 : 0;

    cpuWriteReg(R_A, result);
    cpuSetFlags(flags);
}

void asmRrca(CPU * p_cpu) {
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
void asmCpl(CPU * p_cpu) {
    CPU_FLAGS flags = {-1, 1, 1, -1};
    p_cpu->regs.a = ~p_cpu->regs.a;
    cpuSetFlags(flags);
}

void asmCcf(CPU * p_cpu) {
    uint8_t c = !cpuGetFlag(CARRY_FLAG);
    CPU_FLAGS flags = {-1, 0, 0, c};
    cpuSetFlags(flags);
}

void asmHalt(CPU * p_cpu) {
    p_cpu->halted = true;
}

void asmStop(CPU * p_cpu) {
    //return;
    //cpuWriteReg(R_PC, (p_cpu->regs.pc+2));
    exit(-1);
}

void asmLdh(CPU * p_cpu) {
    /* Copy the byte at address $FF00 + C into register A */
    if(p_cpu->instruction->r1 == R_A) {
        cpuWriteReg(R_A, busReadAddr(0xFF00 | p_cpu->data));
    }
    /* Copy value in register A into the byte pointed by R1 */
    else {
        busWriteAddr(p_cpu->memory_destination, p_cpu->regs.a);
    }

    gbTick(1);
}

void asmCp(CPU * p_cpu) {
    CPU_FLAGS flags = {-1, 1, -1, -1};
    uint8_t reg = p_cpu->regs.a;
    uint8_t data = p_cpu->data;
    int32_t compare = (int32_t)reg - (int32_t)data;

    flags.z = (compare == 0) ? 1 : 0;
    flags.h = ((reg & 0x0F) < (data & 0x0F)) ? 1 : 0;
    flags.c = (data > reg) ? 1 : 0;

    cpuSetFlags(flags);
}

void asmEi(CPU * p_cpu) {
    p_cpu->enabling_ime = true;
}

void asmDi(CPU * p_cpu) {
    p_cpu->int_enable = false;
}

void asmAnd(CPU * p_cpu) {
    CPU_FLAGS flags = {-1,0,1,0};
    uint8_t val = (p_cpu->regs.a & p_cpu->data);

    if(val == 0x00) {
        flags.z = 1;
    }
    cpuWriteReg(R_A, val);
    cpuSetFlags(flags);
}

void asmOr(CPU * p_cpu) {
    CPU_FLAGS flags = {0,0,0,0};
    uint8_t val = (p_cpu->regs.a | p_cpu->data);

    if(val == 0x00) {
        flags.z = 1;
    }
    cpuWriteReg(R_A, val);
    cpuSetFlags(flags);
}

void asmXor(CPU * p_cpu) {
    CPU_FLAGS flags = {0,0,0,0};
    uint8_t val = p_cpu->regs.a ^ (p_cpu->data & 0xFF);

    flags.z = (val == 0) ? 1 : 0;
    cpuWriteReg(R_A, val);
    cpuSetFlags(flags);
}

void asmAdc(CPU * p_cpu) {
    CPU_FLAGS flags = {-1, 0, -1, cpuGetFlag(CARRY_FLAG)};

    uint16_t a = cpuReadReg(R_A);
    uint16_t r8 = p_cpu->data;
    uint16_t val = a + r8 + flags.c;

    flags.z = ((val & 0xFF) == 0x00) ? 1 : 0;
    flags.h = ((a & 0x0F) + (r8 & 0x0F) + flags.c) > 0x0F ? 1 : 0;
    flags.c = (val > 0xFF) ? 1 : 0;

    cpuWriteReg(R_A, val & 0xFF);
    cpuSetFlags(flags);
}

void asmSbc(CPU * p_cpu) {
    CPU_FLAGS flags = {-1, 1, -1, cpuGetFlag(CARRY_FLAG)};
    uint8_t reg_a = cpuReadReg(R_A);
    uint8_t result = 0;
    if(p_cpu->instruction->r2 == R_HL || p_cpu->instruction->r2 == R_NONE) {
        // SBC A [HL] & SBC A n8 require additional cycles
        gbTick(1);
    }

    // Subtract value in r8/[hl]/n8 and the carry flag from A
    result = reg_a - (flags.c + p_cpu->data);
    cpuWriteReg(R_A, result);

    // Flags...
    flags.z = (result == 0) ? 1 : 0;
    flags.h = ((reg_a & 0xF) < ((p_cpu->data & 0xF) + flags.c)) ? 1 : 0;
    flags.c = ((p_cpu->data + flags.c) > reg_a) ? 1 : 0;
    cpuSetFlags(flags);
}

void asmPop(CPU * p_cpu) {
    uint16_t lo = stackPop8();
    gbTick(1);
    uint16_t hi = stackPop8();
    gbTick(1);

    uint16_t val = (hi << 8) | lo;
    if(p_cpu->instruction->r1 == R_AF) {
        val = val & 0xFFF0;
    }
    
    cpuWriteReg(p_cpu->instruction->r1, val);
}

void asmPush(CPU * p_cpu) {
    uint16_t hi = (cpuReadReg(p_cpu->instruction->r1) >> 8) & 0xFF;
    gbTick(1);
    stackPush8(hi);
    
    uint16_t lo = (cpuReadReg(p_cpu->instruction->r1) & 0xFF);
    gbTick(1);
    stackPush8(lo);

    gbTick(1);
}

void asmRet(CPU * p_cpu) {
    /* RET is similar to a POP PC.
       POP value from stack, assign to pc register 
    */
    if(p_cpu->instruction->condition != C_NONE) {
        gbTick(1);
    }
    // This is prob what's wrong.
    //uint16_t pc = stackPop16();
    if(asmCheckCondition(p_cpu)) {
        uint16_t lo = stackPop8();
        uint16_t hi = stackPop8();
        //printf("lo: 0x%02X hi: 0x%02X\n", lo, hi);
        uint16_t val = (hi << 8) | lo;
        //printf("\t0x%04X\n", val);
        //p_cpu->regs.pc = val;
        cpuWriteReg(R_PC, val);
        gbTick(3);
    }
}

void asmScf(CPU * p_cpu) {
    CPU_FLAGS flags = {-1, 0, 0, 1};
    cpuSetFlags(flags);
}

CPU_REGISTER_ENUM cb_reg_order_lookup[] = {
    R_B,
    R_C,
    R_D,
    R_E,
    R_H,
    R_L,
    R_HL,
    R_A
};

void asmCb(CPU * p_cpu) {
    CPU_FLAGS flags = {-1, -1. -1. -1};
    /* This is annoying. Extends zilog z80 instr set 
    $CB $xx, where xx is how the instruction is found. read as next byte after CB op code */
    uint8_t xx = p_cpu->data;
    /* the registers cycle starting from b->a and go from 0 to 7 (0 to 111)*/
    CPU_REGISTER_ENUM cb_reg = ((xx & 0b111) > 0b111) ? R_NONE : cb_reg_order_lookup[xx & 0b111];
    
    /* the bit being changes is from the middle 3 bits */
    uint8_t bit = (xx >> 3) & 0b111;
    
    /* the operation is from the highest 2 bits */
    uint8_t op = (xx >> 6) & 0b11;
    
    /* get the current value in the register */
    uint8_t reg_val = cpuReadRegCb(cb_reg);

    printf("\t\t0xCB%02X bit: %i op: %i register: %i\n", xx, bit, op, cb_reg);

    if(cb_reg == R_HL) {
        gbTick(3);
    }
    else {
        gbTick(1);
    }
    //z n h c
    switch(op) {
        case I_CB_BIT:
            flags.z = !(reg_val & (1 << bit));
            flags.n = 0;
            flags.h = 1;
            cpuSetFlags(flags);
            return;
        case I_CB_RES:
            printf("\t\t%i %i\n", cb_reg, reg_val);
            reg_val &= ~(1 << bit);
            printf("\t\t%i %i\n", cb_reg, reg_val);
            cpuWriteRegCb(cb_reg, reg_val);
            return;
        case I_CB_SET:
            reg_val |= (1 << bit);
            cpuWriteRegCb(cb_reg, reg_val);
            return;
    }

    uint8_t val, msb, lsb, cf_new;
    bool cf = cpuGetFlag(CARRY_FLAG);

    switch(bit) {
        case I_CB_RLC: // RLC
            /* Rotate Left Circular -- MSB goes to carry & LSB */
            val = cpuReadRegCb(cb_reg);
            msb = (val >> 7) & 1;
            cf_new = msb;
            val = (val << 1) | msb;

            flags.z = (val == 0) ? 1 : 0;
            flags.n = 0;
            flags.h = 0;
            flags.c = cf_new;
            cpuSetFlags(flags);
            cpuWriteRegCb(cb_reg, val);
            return;
        case I_CB_RRC: // RRC
            /* Rotate Right Circular -- lsb goes to carry & MSB */
            val = cpuReadRegCb(cb_reg);
            lsb = val & 0x01;
            cf_new = lsb;
            val = (val >> 1) | (lsb << 7);

            flags.z = (val == 0) ? 1 : 0;
            flags.n = 0;
            flags.h = 0;
            flags.c = cf_new;
            cpuSetFlags(flags);
            cpuWriteRegCb(cb_reg, val);
            return;
        case I_CB_RL: // RL
            /* Rotate Left through Carry */
            val = cpuReadRegCb(cb_reg);
            msb = (val >> 7) & 1;
            cf_new = msb;
            val = (val << 1) | cf;

            flags.z = (val == 0) ? 1 : 0;
            flags.n = 0;
            flags.h = 0;
            flags.c = cf_new;
            cpuSetFlags(flags);
            cpuWriteRegCb(cb_reg, val);
            return;
        case I_CB_RR: // RR
            /* Rotate right through carry */
            val = cpuReadRegCb(cb_reg);
            lsb = val & 0x01;
            cf_new = lsb;
            val = (val >> 1) | (cf << 7);
            
            flags.z = (val == 0) ? 1 : 0;
            flags.n = 0;
            flags.h = 0;
            flags.c = cf_new;            
            cpuSetFlags(flags);
            cpuWriteRegCb(cb_reg, val);
            return;
        case I_CB_SLA: // SLA
            /* Shift left arithmetically */
            val = cpuReadRegCb(cb_reg);
            msb = (val >> 7) & 1;
            cf_new = msb;
            val = (val << 1);

            flags.z = (val == 0) ? 1 : 0;
            flags.n = 0;
            flags.h = 0;
            flags.c = cf_new;
            cpuSetFlags(flags);
            cpuWriteRegCb(cb_reg, val);
            return;
        case I_CB_SRA: // SRA
            /* Shift right arithmetically */
            val = cpuReadRegCb(cb_reg);
            lsb = val & 1;
            msb = (val >> 7) & 1;
            cf_new = lsb;
            /* msb must be added back into b7 */
            val = (val >> 1) | (msb << 7);

            flags.z = (val == 0) ? 1 : 0;
            flags.n = 0;
            flags.h = 0;
            flags.c = cf_new;
            cpuSetFlags(flags);
            cpuWriteRegCb(cb_reg, val);
        case I_CB_SWAP: // SWAP
            val = cpuReadRegCb(cb_reg);
            val = ((val & 0x0F) << 4) | ((val & 0xF0) >> 4);

            flags.z = (val == 0) ? 1 : 0;
            flags.n = 0;
            flags.h = 0;
            flags.c = 0;
            cpuSetFlags(flags);
            cpuWriteRegCb(cb_reg, val);
            return;
        case I_CB_SRL: // SRL
            //TODO Finish this
            val = cpuReadRegCb(cb_reg);
            lsb = val & 1;
            cf_new = lsb;
            /* msb should always be 0 in this case? */
            val = (val >> 1) & (0 << 7);

            flags.z = (val == 0) ? 1 : 0;
            flags.n = 0;
            flags.h = 0;
            flags.c = cf_new;
            cpuSetFlags(flags);
            cpuWriteRegCb(cb_reg, val);
            return;
    }
}

void asmDaa(CPU * p_cpu) {
    CPU_FLAGS flags = {-1, -1, -1, -1};
    cpuGetFlags(&flags.z, &flags.n, &flags.h, &flags.c);
    uint8_t reg = cpuReadReg(R_A);
    uint16_t adjustment = 0;
    uint8_t new_c = flags.c;

    // if flags.n == 1, adjustments are based on h & c 
    if(flags.n) {
        if(flags.h) {
            adjustment |= 0x06;
        }
        if(flags.c) {
            adjustment |= 0x60;
        }
        reg = reg - adjustment;
    }
    // if flags.n == 0, we are doing addition.
    else {
        if(flags.h || ((reg & 0x0F) > 0x09)) {
            adjustment |= 0x06;
        }
        if(flags.c || (reg > 0x99)) {
            adjustment |= 0x60;
            new_c = 1;
        }
        reg = reg + adjustment;
    }

    flags.z = (reg == 0) ? 1 : 0;
    flags.h = 0;
    flags.c = new_c;

    cpuWriteReg(R_A, reg);
    cpuSetFlags(flags);
}

void asmReti(CPU * p_cpu) {
    // enable interrupts
    p_cpu->int_enable = true;
    asmRet(p_cpu);
}

void asmRra(CPU * p_cpu) {
    CPU_FLAGS flags = {0,0,0,0};
    uint8_t a = cpuReadReg(R_A);
    uint8_t new_c = a & 0x01;
    uint8_t c = cpuGetFlag(CARRY_FLAG);
    a = (a >> 1) | (c << 7);
    cpuWriteReg(R_A, a);
    flags.c = new_c;
    cpuSetFlags(flags);
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
    [I_RLCA] = asmRlca,
    [I_RRCA] = asmRrca,
    [I_RRA] = asmRra,
    [I_RLA] = asmRla,
    [I_HALT] = asmHalt,
    [I_SBC] = asmSbc,
    [I_POP] = asmPop,
    [I_PUSH] = asmPush,
    [I_RET] = asmRet,
    [I_RETI] = asmReti,
    [I_CB] = asmCb,
    [I_DAA] = asmDaa,
    [I_CCF] = asmCcf,
    [I_CPL] = asmCpl,
    [I_RST] = asmRst,
    [I_SCF] = asmScf
};

ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return asm_functions[i];
}

void asmSetCpuPtr(CPU * cpu) {
    //p_cpu = cpu;

    //printf("\t\t\tcheck: 0x%02X\n", p_cpu->regs.a);
}