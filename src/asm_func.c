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

void asmRst(void) {
    gotoAddr(p_cpu->instruction->parameter, true);
}

void asmNone(void) {
    return;
}

void asmNop(void) {
    return;
}

void asmLd(void) { 
    CPU_FLAGS flags = {-1,-1,-1,-1};
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
        uint8_t h_flag = (cpuReadReg(p_cpu->instruction->r2) & 0xF) + (p_cpu->data & 0xF) >= 0x10;
        uint8_t c_flag = (cpuReadReg(p_cpu->instruction->r2) & 0xFF) + (p_cpu->data &0xFF) >= 0x100;
        
        // z n h c
        flags.z = 0;
        flags.n = 0;
        flags.h = h_flag;
        flags.c = c_flag;
        cpuSetFlags(flags);
        
        // this is the stack ptr
        uint16_t r2_val = cpuReadReg(p_cpu->instruction->r2);
        // write stack ptr + e8 (signed int8) to r1
        cpuWriteReg(p_cpu->instruction->r1, r2_val + (int8_t)p_cpu->data);
        gbTick(2);
        //printf("WRITING 0x%04X\n", r2_val + (int8_t)p_cpu->data);
        return;
    }

    cpuWriteReg(p_cpu->instruction->r1, p_cpu->data);
}

void asmInc(void) {
    CPU_FLAGS flags = {-1, 0, -1, -1};
    uint16_t val = 0;
    uint16_t val_new = 0;
    bool is_16bit = (p_cpu->instruction->r1 >= R_AF) ? true : false;

    /* INC r8 */
    if(is_16bit == false) {
        //val = p_cpu->data + 1;
        val = cpuReadReg(p_cpu->instruction->r1);
        cpuWriteReg(p_cpu->instruction->r1, (val + 1) & 0xFF);

        if(((val + 1) & 0xFF) == 0x0) {
            flags.z = 1;
        }
        if((val & 0x0F) == 0x0F) {
            flags.h = 1;
        }
        cpuSetFlags(flags);
    }
    else {
        /* Cycle for extra byte */
        gbTick(1);
        /* INC [HL] */
        if(p_cpu->instruction->r1 == R_HL && p_cpu->to_memory == true) {
            val = busReadAddr(cpuReadReg(R_HL));
            val = val & 0xFF;
            val_new = (val + 1) & 0xFF;
            busWriteAddr(cpuReadReg(R_HL), val + 1);

            /* 3 total cycles for INC [HL] */
            gbTick(1);

            if((val + 1) == 0x00) {
                flags.z = 1;
            }
            if((val & 0x0F) == 0x0F) {
                flags.h = 1;
            }
            cpuSetFlags(flags);
        }
        /* INC r16 */
        else {
            val = p_cpu->data + 1;
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
        val = cpuReadReg(p_cpu->instruction->r1) & 0xFF;
        cpuWriteReg(p_cpu->instruction->r1, (val - 1) & 0xFF);

        if((val - 1) == 0x0) {
            flags.z = 1;
        }
        if((val & 0x0F) == 0x00) {
            flags.h = 1;
        }
        cpuSetFlags(flags);
    }
    else {
        /* Cycle for extra byte */
        gbTick(1);
        /* DEC [HL] */
        if(p_cpu->instruction->r1 == R_HL && p_cpu->to_memory == true) {
            val = busReadAddr(cpuReadReg(R_HL)) & 0xFF;
            busWriteAddr(cpuReadReg(R_HL), val - 1);

            /* 3 total cycles for INC [HL] */
            gbTick(1);

            if((val - 1) == 0x00) {
                flags.z = 1;
            }
            if((val & 0x0F) == 0x00) {
                flags.h = 1;
            }
            cpuSetFlags(flags);
        }
        /* DEC r16 */
        else {
            val = cpuReadReg(p_cpu->instruction->r1);
            cpuWriteReg(p_cpu->instruction->r1, val - 1);
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
        int8_t signed_data = p_cpu->data;
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

void asmSub(void) {
    CPU_FLAGS flags = {-1, 1, -1, -1};

    uint8_t result = p_cpu->regs.a - p_cpu->data;

    flags.z = (result == 0) ? 1 : 0;
    flags.h = ((p_cpu->regs.a & 0x0F) < (p_cpu->data & 0x0F)) ? 1 : 0;
    flags.c = (p_cpu->data > p_cpu->regs.a) ? 1 : 0;

    cpuWriteReg(R_A, result);
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
    exit(-1);
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
    p_cpu->enabling_ime = true;
}

void asmDi() {
    p_cpu->int_enable = false;
}

void asmAnd(void) {
    CPU_FLAGS flags = {-1,0,1,0};
    uint8_t val = (p_cpu->regs.a & p_cpu->data);

    if(val == 0x00) {
        flags.z = 1;
    }
    cpuWriteReg(p_cpu->instruction->r1, val);
    cpuSetFlags(flags);
}

void asmOr(void) {
    CPU_FLAGS flags = {0,0,0,0};
    uint8_t val = (p_cpu->regs.a | p_cpu->data);

    if(val == 0x00) {
        flags.z = 1;
    }
    cpuWriteReg(p_cpu->instruction->r1, val);
    cpuSetFlags(flags);
}

void asmXor(void) {
    CPU_FLAGS flags = {0,0,0,0};
    uint8_t val = p_cpu->regs.a ^ (p_cpu->data & 0xFF);
    flags.z = (p_cpu->regs.a == 0) ? 1 : 0;
    cpuSetFlags(flags);
    cpuWriteReg(p_cpu->instruction->r1, val);
}

void asmAdc(void) {
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
    uint16_t lo = stackPop8();
    uint16_t hi = stackPop8();
    uint16_t val = (hi << 8) | lo;

    if(p_cpu->instruction->r1 == R_AF) {
        val = val & 0xFFF0;
    }
    
    cpuWriteReg(p_cpu->instruction->r1, val);
}

void asmPush(void) {
    uint16_t hi = (cpuReadReg(p_cpu->instruction->r1) >> 8) & 0xFF;
    uint16_t lo = (cpuReadReg(p_cpu->instruction->r1) & 0xFF);

    stackPush8(hi);
    stackPush8(lo);
    gbTick(3);
}

void asmRet(void) {
    /* RET is similar to a POP PC.
       POP value from stack, assign to pc register 
    */
    if(p_cpu->instruction->condition != C_NONE) {
        gbTick(1);
    }
    // This is prob what's wrong.
    //uint16_t pc = stackPop16();
    if(asmCheckCondition()) {
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

void asmCb(void) {
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

void asmDaa(void) {
    CPU_FLAGS flags = {0};
    // initialize adjustment to zero (both cases)
    int8_t adj = 0;
    uint8_t a = cpuReadReg(R_A);
    cpuGetFlags(&flags.z, &flags.n, &flags.h, &flags.c);

    // if subtract flag is set
    if(flags.n) {
        // add $6 if half carry is set, add $60 if carry is set
        adj = (flags.h * 0x6) + (flags.c * 0x60);
        a = a - adj;
    }
    else if(flags.n == 0) {
        if(flags.h || ((a & 0xF) > 0x9)) {
            adj = adj + 0x6;
        }
        if(flags.c || a > 0x99) {
            adj = adj + 0x60;
        }
        a = a + adj;
        flags.c = (a > 0x99) ? 1 : 0;
    }

    flags.z = (a == 0) ? 1 : 0;
    flags.h = 0;
    cpuSetFlags(flags);
    cpuWriteReg(R_A, a);
}

void asmReti(void) {
    // enable interrupts
    p_cpu->int_enable = true;
    // return from subroutine
    p_cpu->regs.pc = stackPop16();
    // No need to check for cc, just do 3 cycles
    gbTick(3);
}

void asmRra(void) {
    CPU_FLAGS flags = {0,0,0,-1};
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
    [I_RST] = asmRst
};

ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i) {
    return asm_functions[i];
}

void asmSetCpuPtr(CPU * cpu) {
    p_cpu = cpu;

    printf("\t\t\tcheck: 0x%02X\n", p_cpu->regs.a);
}