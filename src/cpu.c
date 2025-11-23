#include "cpu.h"
#include "bus.h"
#include "gb.h"
#include "memory.h"
#include "stack.h"
#include "asm_func.h"
#include "debug_blarg.h"

#define CPU_NUM_INT 5

static CPU cpu;

static CPU_INTERRUPTS cpu_interrupts[CPU_NUM_INT] = {
    {INT_VBLANK, 0x40},
    {INT_LCD_STAT, 0x48},
    {INT_TIMER, 0x50},
    {INT_SERIAL, 0x58},
    {INT_JOYPAD, 0x60}
};

static CPU_INSTRUCTION instruction_set[0xFF + 0x01] = {
    /* First Row */
    [0x00] = {I_NOP,  M_NONE                      },
    [0x01] = {I_LD,   M_REG_D16,      R_BC        },
    [0x02] = {I_LD,   M_MEMREG_REG,   R_BC,    R_A},
    [0x03] = {I_INC,  M_REG,          R_BC        },
    [0x04] = {I_INC,  M_REG,          R_B         },
    [0x05] = {I_DEC,  M_REG,          R_B         },
    [0x06] = {I_LD,   M_REG_D8,       R_B         },
    [0x07] = {I_RLCA                              },
    [0x08] = {I_LD,   M_A16_REG,      R_NONE, R_SP},
    [0x09] = {I_ADD,  M_REG_REG,      R_HL,   R_BC},
    [0x0A] = {I_LD,   M_REG_MEMREG,   R_A,    R_BC},
    [0x0B] = {I_DEC,  M_REG,          R_BC        },
    [0x0C] = {I_INC,  M_REG,          R_C         },
    [0x0D] = {I_DEC,  M_REG,          R_C         },
    [0x0E] = {I_LD,   M_REG_D8,       R_C         },
    [0x0F] = {I_RRCA                              },
    /* Second Row */
    [0x10] = {I_STOP, M_D8                        },
    [0x11] = {I_LD,   M_REG_D16,      R_DE        },
    [0x12] = {I_LD,   M_MEMREG_REG,   R_DE,    R_A},
    [0x13] = {I_INC,  M_REG,          R_DE        },
    [0x14] = {I_INC,  M_REG,          R_D         },
    [0x15] = {I_DEC,  M_REG,          R_D         },
    [0x16] = {I_LD,   M_REG,          R_D         },
    [0x17] = {I_RLA                               },
    [0x18] = {I_JR,   M_D8                       },
    [0x19] = {I_ADD,  M_REG_REG,      R_HL,   R_DE},
    [0x1A] = {I_LD,   M_REG_MEMREG,   R_A,    R_DE},
    [0x1B] = {I_DEC,  M_REG,          R_DE        },
    [0x1C] = {I_INC,  M_REG,          R_E         },
    [0x1D] = {I_DEC,  M_REG,          R_E         },
    [0x1E] = {I_LD,   M_REG_D8,       R_E         },
    [0x1F] = {I_SRA                               },
    /* Third Row */
    [0x20] = {I_JR, M_D8, R_NONE, R_NONE, C_NZ},
    [0x21] = {I_LD, M_REG_D16, R_HL},
    [0x22] = {I_LD, M_HLI_R, R_HL, R_A},
    [0x23] = {I_INC, M_REG, R_HL},
    [0x24] = {I_INC, M_REG, R_H},
    [0x25] = {I_DEC, M_REG, R_H},
    [0x26] = {I_LD, M_REG_D8, R_H},
    [0x27] = {I_DAA},
    [0x28] = {I_JR, M_D8, R_NONE, R_NONE, C_NZ},
    [0x29] = {I_ADD, M_REG_REG, R_HL, R_HL},
    [0x2A] = {I_LD, M_REG_HLI, R_A, R_HL},
    [0x2B] = {I_DEC, M_REG, R_HL},
    [0x2C] = {I_INC, M_REG, R_L},
    [0x2D] = {I_DEC, M_REG, R_L},
    [0x2E] = {I_LD, M_REG_D8, R_L},
    [0x2F] = {I_CPL},
    /* Fourth Row */
    [0x30] = {I_JR, M_D8, R_NONE, R_NONE, C_NC},
    [0x31] = {I_LD, M_REG_D16, R_SP},
    [0x32] = {I_LD, M_HLD_R, R_HL, R_A},
    [0x33] = {I_INC, M_REG, R_SP},
    [0x34] = {I_INC, M_MEMREG, R_HL},
    [0x35] = {I_DEC, M_MEMREG, R_HL},
    [0x36] = {I_LD, M_MEMREG_D8, R_HL},
    [0x37] = {I_SCF},
    [0x38] = {I_JR, M_D8, R_NONE, R_NONE, C_C},
    [0x39] = {I_ADD, M_REG_REG, R_HL, R_SP},
    [0x3A] = {I_LD, M_REG_HLD, R_A, R_HL},
    [0x3B] = {I_DEC, M_REG, R_SP},
    [0x3C] = {I_INC, M_REG, R_A},
    [0x3D] = {I_DEC, M_REG, R_A},
    [0x3E] = {I_LD, M_REG_D8, R_A},
    [0x3F] = {I_CCF},
    /* Fifth Row */
    [0x40] = {I_LD, M_REG_REG, R_B, R_B},
    [0x41] = {I_LD, M_REG_REG, R_B, R_C},
    [0x42] = {I_LD, M_REG_REG, R_B, R_D},
    [0x43] = {I_LD, M_REG_REG, R_B, R_E},
    [0x44] = {I_LD, M_REG_REG, R_B, R_H},
    [0x45] = {I_LD, M_REG_REG, R_B, R_L},
    [0x46] = {I_LD, M_REG_MEMREG, R_B, R_HL},
    [0x47] = {I_LD, M_REG_REG, R_B, R_A},
    [0x48] = {I_LD, M_REG_REG, R_C, R_B},
    [0x49] = {I_LD, M_REG_REG, R_C, R_C},
    [0x4A] = {I_LD, M_REG_REG, R_C, R_D},
    [0x4B] = {I_LD, M_REG_REG, R_C, R_E},
    [0x4C] = {I_LD, M_REG_REG, R_C, R_H},
    [0x4D] = {I_LD, M_REG_REG, R_C, R_L},
    [0x4E] = {I_LD, M_REG_MEMREG, R_C, R_HL},
    [0x4F] = {I_LD, M_REG_REG, R_C, R_A},
    /* Sixth Row */
    [0x50] = {I_LD, M_REG_REG, R_D, R_B},
    [0x51] = {I_LD, M_REG_REG, R_D, R_C},
    [0x52] = {I_LD, M_REG_REG, R_D, R_D},
    [0x53] = {I_LD, M_REG_REG, R_D, R_E},
    [0x54] = {I_LD, M_REG_REG, R_D, R_H},
    [0x55] = {I_LD, M_REG_REG, R_D, R_L},
    [0x56] = {I_LD, M_REG_MEMREG, R_D, R_HL},
    [0x57] = {I_LD, M_REG_REG, R_D, R_A},
    [0x58] = {I_LD, M_REG_REG, R_E, R_B},
    [0x59] = {I_LD, M_REG_REG, R_E, R_C},
    [0x5A] = {I_LD, M_REG_REG, R_E, R_C},
    [0x5B] = {I_LD, M_REG_REG, R_E, R_E},
    [0x5C] = {I_LD, M_REG_REG, R_E, R_H},
    [0x5D] = {I_LD, M_REG_REG, R_E, R_L},
    [0x5E] = {I_LD, M_REG_MEMREG, R_E, R_HL},
    [0x5F] = {I_LD, M_REG_REG, R_E, R_A},
    /* Seventh Row */
    [0x60] = {I_LD, M_REG_REG, R_H, R_B},
    [0x61] = {I_LD, M_REG_REG, R_H, R_C},
    [0x62] = {I_LD, M_REG_REG, R_H, R_D},
    [0x63] = {I_LD, M_REG_REG, R_H, R_E},
    [0x64] = {I_LD, M_REG_REG, R_H, R_H},
    [0x65] = {I_LD, M_REG_REG, R_H, R_L},
    [0x66] = {I_LD, M_REG_MEMREG, R_H, R_HL},
    [0x67] = {I_LD, M_REG_REG, R_H, R_A},
    [0x68] = {I_LD, M_REG_REG, R_L, R_B},
    [0x69] = {I_LD, M_REG_REG, R_L, R_C},
    [0x6A] = {I_LD, M_REG_REG, R_L, R_D},
    [0x6B] = {I_LD, M_REG_REG, R_L, R_E},
    [0x6C] = {I_LD, M_REG_REG, R_L, R_H},
    [0x6D] = {I_LD, M_REG_REG, R_L, R_L},
    [0x6E] = {I_LD, M_REG_MEMREG, R_L, R_HL},
    [0x6F] = {I_LD, M_REG_REG, R_L, R_A},
    /* Eighth Row */
    [0x70] = {I_LD, M_MEMREG_REG, R_HL, R_B},
    [0x71] = {I_LD, M_MEMREG_REG, R_HL, R_C},
    [0x72] = {I_LD, M_MEMREG_REG, R_HL, R_D},
    [0x73] = {I_LD, M_MEMREG_REG, R_HL, R_E},
    [0x74] = {I_LD, M_MEMREG_REG, R_HL, R_H},
    [0x75] = {I_LD, M_MEMREG_REG, R_HL, R_L},
    [0x76] = {I_HALT},
    [0x77] = {I_LD, M_MEMREG_REG, R_HL, R_A},
    [0x78] = {I_LD, M_REG_REG, R_A, R_B},
    [0x79] = {I_LD, M_REG_REG, R_A, R_C},
    [0x7A] = {I_LD, M_REG_REG, R_A, R_D},
    [0x7B] = {I_LD, M_REG_REG, R_A, R_E},
    [0x7C] = {I_LD, M_REG_REG, R_A, R_L},
    [0x7D] = {I_LD, M_REG_REG, R_A, R_L},
    [0x7E] = {I_LD, M_REG_MEMREG, R_A, R_HL},
    [0x7F] = {I_LD, M_REG_REG, R_A, R_A},
    /* NINTH ROW */
    [0x80] = {I_ADD, M_REG_REG, R_A, R_B},
    [0x81] = {I_ADD, M_REG_REG, R_A, R_C},
    [0x82] = {I_ADD, M_REG_REG, R_A, R_D},
    [0x83] = {I_ADD, M_REG_REG, R_A, R_E},
    [0x84] = {I_ADD, M_REG_REG, R_A, R_H},
    [0x85] = {I_ADD, M_REG_REG, R_A, R_L},
    [0x86] = {I_ADD, M_REG_MEMREG, R_A, R_H},
    [0x87] = {I_ADD, M_REG_REG, R_A, R_A},
    [0x88] = {I_ADC, M_REG_REG, R_A, R_B},
    [0x89] = {I_ADC, M_REG_REG, R_A, R_C},
    [0x8A] = {I_ADC, M_REG_REG, R_A, R_D},
    [0x8B] = {I_ADC, M_REG_REG, R_A, R_E},
    [0x8C] = {I_ADC, M_REG_REG, R_A, R_H},
    [0x8D] = {I_ADC, M_REG_REG, R_A, R_L},
    [0x8E] = {I_ADC, M_REG_REG, R_A, R_HL},
    [0x8F] = {I_ADC, M_REG_REG, R_A, R_A},
    /* TENTH ROW */
    [0x90] = {I_SUB, M_REG_REG, R_A, R_B},
    [0x91] = {I_SUB, M_REG_REG, R_A, R_C},
    [0x92] = {I_SUB, M_REG_REG, R_A, R_D},
    [0x93] = {I_SUB, M_REG_REG, R_A, R_E},
    [0x94] = {I_SUB, M_REG_REG, R_A, R_H},
    [0x95] = {I_SUB, M_REG_REG, R_A, R_L},
    [0x96] = {I_SUB, M_REG_MEMREG, R_A, R_HL},
    [0x97] = {I_SUB, M_REG_REG, R_A, R_A},
    [0x98] = {I_SBC, M_REG_REG, R_A, R_B},
    [0x99] = {I_SBC, M_REG_REG, R_A, R_C},
    [0x9A] = {I_SBC, M_REG_REG, R_A, R_D},
    [0x9B] = {I_SBC, M_REG_REG, R_A, R_E},
    [0x9C] = {I_SBC, M_REG_REG, R_A, R_H},
    [0x9D] = {I_SBC, M_REG_REG, R_A, R_L},
    [0x9E] = {I_SBC, M_REG_MEMREG, R_A, R_HL},
    [0x9F] = {I_SBC, M_REG_REG, R_A, R_A},
    /* ELEVENTH ROW */
    [0xA0] = {I_AND, M_REG_REG, R_A, R_B},
    [0xA1] = {I_AND, M_REG_REG, R_A, R_C},
    [0xA2] = {I_AND, M_REG_REG, R_A, R_D},
    [0xA3] = {I_AND, M_REG_REG, R_A, R_E},
    [0xA4] = {I_AND, M_REG_REG, R_A, R_H},
    [0xA5] = {I_AND, M_REG_REG, R_A, R_L},
    [0xA6] = {I_AND, M_REG_MEMREG, R_A, R_HL},
    [0xA7] = {I_AND, M_REG_REG, R_A, R_A},
    [0xA8] = {I_XOR, M_REG_REG, R_A, R_B},
    [0xA9] = {I_XOR, M_REG_REG, R_A, R_C},
    [0xAA] = {I_XOR, M_REG_REG, R_A, R_D},
    [0xAB] = {I_XOR, M_REG_REG, R_A, R_E},
    [0xAC] = {I_XOR, M_REG_REG, R_A, R_H},
    [0xAD] = {I_XOR, M_REG_REG, R_A, R_L},
    [0xAE] = {I_XOR, M_REG_MEMREG, R_A, R_HL},
    [0xAF] = {I_XOR, M_REG_REG, R_A, R_A},
    /* ROW TWELVE */
    [0xB0] = {I_OR, M_REG_REG, R_A, R_B},
    [0xB1] = {I_OR, M_REG_REG, R_A, R_C},
    [0xB2] = {I_OR, M_REG_REG, R_A, R_D},
    [0xB3] = {I_OR, M_REG_REG, R_A, R_E},
    [0xB4] = {I_OR, M_REG_REG, R_A, R_H},
    [0xB5] = {I_OR, M_REG_REG, R_A, R_L},
    [0xB6] = {I_OR, M_REG_MEMREG, R_A, R_HL},
    [0xB7] = {I_OR, M_REG_REG, R_A, R_A},
    [0xB8] = {I_CP, M_REG_REG, R_A, R_B},
    [0xB9] = {I_CP, M_REG_REG, R_A, R_C},
    [0xBA] = {I_CP, M_REG_REG, R_A, R_D},
    [0xBB] = {I_CP, M_REG_REG, R_A, R_E},
    [0xBC] = {I_CP, M_REG_REG, R_A, R_H},
    [0xBD] = {I_CP, M_REG_REG, R_A, R_L},
    [0xBE] = {I_CP, M_REG_MEMREG, R_A, R_HL},
    [0xBF] = {I_CP, M_REG_REG, R_A, R_A},
    /* ROW THIRTEEN */
    [0xC0] = {I_RET, M_NONE, R_NONE, R_NONE, C_NZ},
    [0xC1] = {I_POP, M_REG, R_BC},
    [0xC2] = {I_JP, M_D16, R_NONE, R_NONE, C_NZ},
    [0xC3] = {I_JP, M_D16},
    [0xC4] = {I_CALL, M_D16, R_NONE, R_NONE, C_NZ},
    [0xC5] = {I_PUSH, M_REG, R_BC},
    [0xC6] = {I_ADD, M_REG_D8, R_A},
    [0xC7] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x00},
    [0xC8] = {I_RET, M_NONE, R_NONE, R_NONE, C_Z},
    [0xC9] = {I_RET},
    [0xCA] = {I_JP, M_D16, R_NONE, R_NONE, C_Z},
    [0xCB] = {I_CB, M_D8},
    [0xCC] = {I_CALL, M_D16, R_NONE, R_NONE, C_Z},
    [0xCD] = {I_CALL, M_D16},
    [0xCE] = {I_ADC, M_REG_D8, R_A},
    [0xCF] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x08},
    /* ROW FOURTEEN */
    [0xD0] = {I_RET, M_NONE, R_NONE, R_NONE, C_NC},
    [0xD1] = {I_POP, M_REG, R_DE},
    [0xD2] = {I_JP, M_D16, R_NONE, R_NONE, C_NC},
    [0xD3] = {I_NONE},
    [0xD4] = {I_CALL, M_D16, R_NONE, R_NONE, C_NC},
    [0xD5] = {I_PUSH, M_REG, R_DE},
    [0xD6] = {I_SUB, M_REG_D8, R_A},
    [0xD7] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x10},
    [0xD8] = {I_RET, M_NONE, R_NONE, R_NONE, C_C},
    [0xD9] = {I_RETI},
    [0xDA] = {I_JP, M_D16, R_NONE, R_NONE, C_C},
    [0xDB] = {I_NONE},
    [0xDC] = {I_CALL, M_D16, R_NONE, R_NONE, C_C},
    [0xDD] = {I_NONE},
    [0xDE] = {I_SBC, M_REG_D8, R_A},
    [0xDF] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x18},
    /* ROW FIFTEEN */
    [0XE0] = {I_LDH, M_A8_REG, R_NONE, R_A},
    [0XE1] = {I_POP, M_REG, R_HL},
    [0XE2] = {I_LD, M_MEMREG_REG, R_C, R_A},
    [0XE5] = {I_PUSH, M_REG, R_HL},
    [0XE6] = {I_AND, M_REG_D8, R_SP},
    [0XE7] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x20},
    [0XE8] = {I_ADD, M_REG_D8, R_SP},
    [0XE9] = {I_JP, M_REG, R_HL},
    [0XEA] = {I_LD, M_A16_REG, R_NONE, R_A},
    [0XEE] = {I_XOR, M_REG_D8, R_A},
    [0XEF] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x28},
    /* ROW SIXTEEN */
    [0XF0] = {I_LDH, M_REG_A8, R_A},
    [0XF1] = {I_POP, M_REG, R_AF},
    [0XF2] = {I_LD, M_REG_MEMREG, R_A, R_C},
    [0XF3] = {I_DI},
    [0XF5] = {I_PUSH, M_REG, R_AF},
    [0XF6] = {I_OR, M_REG_D8, R_A},
    [0XF7] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x30},
    [0XF8] = {I_LD, M_HL_SPR, R_HL, R_SP},
    [0XF9] = {I_LD, M_REG_REG, R_SP, R_HL},
    [0XFA] = {I_LD, M_REG_A16, R_A},
    [0XFB] = {I_EI},
    [0XFE] = {I_CP, M_REG_D8, R_A},
    [0XFF] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x38}
};

static char * instruction_set_s[I_SET_SIZE] = {
    "NONE",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RL",
    "RLA",
    "JR",
    "ADD",
    "RRCA",
    "CPL",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "XOR",
    "OR",
    "CP",
    "RET",
    "POP",
    "JP",
    "CALL",
    "PUSH",
    "RST",
    "PREFIX",
    "EI",
    "DI",
    "LDH",
    "RLCA",
    "DAA",
    "STOP",
    "AND",
    "CB",
    "RETI"
};

static char * registers_s[R_PC + 1] = {
    "R_NONE",
    "A",
    "F",
    "B",
    "C",
    "D",
    "E",
    "H",
    "L",
    "AF",
    "BC",
    "DE",
    "HL",
    "SP",
    "PC"
};

CPU_INSTRUCTION * cpuGetInstructionByOpCode(uint16_t op_code) {
    return &instruction_set[op_code];
}

uint16_t cpuReadReg16(uint8_t r1, uint8_t r2) {
    uint16_t reg16 = r1 | (r2 << 8);
    return reg16;
}

uint16_t cpuReadReg(CPU_REGISTER_ENUM reg) {
    switch(reg) {
        case R_A:
            return cpu.regs.a;
        case R_F:
            return cpu.regs.f;
        case R_B:
            return cpu.regs.b;
        case R_C:
            return cpu.regs.c;
        case R_D:
            return cpu.regs.d;
        case R_E:
            return cpu.regs.e;
        case R_H:
            return cpu.regs.h;
        case R_L:
            return cpu.regs.l;
        case R_PC:
            return cpu.regs.pc;
        case R_SP:
            return cpu.regs.sp;
        /* All of these have to be swapped for endian-ness */
        case R_AF:
            return cpuReadReg16(cpu.regs.a, cpu.regs.f);
        case R_BC:
            return cpuReadReg16(cpu.regs.b, cpu.regs.c);
        case R_DE:
            return cpuReadReg16(cpu.regs.d, cpu.regs.e);
        case R_HL:
            return cpuReadReg16(cpu.regs.h, cpu.regs.l);

        default:
            bool r1_undef = false;
            if(cpu.instruction->r1 == 0) {
                r1_undef = true;
            } 
            printf("Register enumeration %i on register %i not found for instruction %s\n", reg, r1_undef, instruction_set_s[cpu.instruction->type]);
            printf("OP Code: 0x%02X\n", cpu.op_code);
            return 0;
    }
}

uint8_t cpuReadRegCb(CPU_REGISTER_ENUM reg) {
    switch(reg) {
        case R_A:
            return cpu.regs.a;
        case R_F:
            return cpu.regs.f;
        case R_B:
            return cpu.regs.b;
        case R_C:
            return cpu.regs.c;
        case R_D:
            return cpu.regs.d;
        case R_E:
            return cpu.regs.e;
        case R_H:
            return cpu.regs.h;
        case R_L:
            return cpu.regs.l;
        case R_HL:
            return busReadAddr(cpuReadReg(reg));
        default:
            exit(-1);
    }
}

void cpuWriteRegCb(CPU_REGISTER_ENUM reg, uint8_t val) {
        switch(reg) {
        case R_A:
            cpu.regs.a = val;
        case R_F:
            cpu.regs.f = val;
        case R_B:
            cpu.regs.b = val;
        case R_C:
            cpu.regs.c = val;
        case R_D:
            cpu.regs.d = val;
        case R_E:
            cpu.regs.e = val;
        case R_H:
            cpu.regs.h = val;
        case R_L:
            cpu.regs.l = val;
        case R_HL:
            busWriteAddr(cpuReadReg(reg), val);
        default:
            exit(-1);
    }
}

static uint16_t cpuSwapEndian(uint16_t val) {
    return ((val & 0x00FF) << 8) | ((val & 0xFF00) >> 8);
}

static void cpuWriteReg16(uint16_t val, uint8_t * r1, uint8_t * r2) {
    *r1 = ((val & 0x00FF) << 8) >> 8;
    *r2 = (val & 0xFF00) >> 8;
}

void cpuWriteReg(CPU_REGISTER_ENUM reg, uint16_t val) {
    switch(reg) {
        /* 8 bit registers */
        case R_A:
            cpu.regs.a = val;
            return;
        case R_F:
            cpu.regs.f = val;
            return;
        case R_B:
            cpu.regs.b = val;
            return;
        case R_C:
            cpu.regs.c = val;
            return;
        case R_D:
            cpu.regs.d = val;
            return;
        case R_E:
            cpu.regs.e = val;
            return;
        case R_H:
            cpu.regs.h = val;
            return;
        case R_L:
            cpu.regs.l = val;
            return;
        /* Begin 16 bit registers */
        case R_AF:
            cpuWriteReg16(val, &cpu.regs.a, &cpu.regs.f);
            return;
        case R_BC:
            cpuWriteReg16(val, &cpu.regs.b, &cpu.regs.c);
            return;
        case R_DE:
            cpuWriteReg16(val, &cpu.regs.d, &cpu.regs.e);
            return;
        case R_HL:
            cpuWriteReg16(val, &cpu.regs.h, &cpu.regs.l);
            return;
        case R_SP:
            cpu.regs.sp = val;
            return;
        case R_PC:
            cpu.regs.pc = val;
            return;
        default:
            cpuSwapEndian(val);
            printf("Incorrect register: %i\n", reg);
            return;
    }
}

static void cpuGetInstruction(void) {
    uint16_t pc = cpuReadReg(R_PC);
    cpu.op_code = busReadAddr(cpu.regs.pc++);
    cpu.instruction = cpuGetInstructionByOpCode(cpu.op_code);

    //printf("PC: 0x%04X (0x%02X) 0x%02X 0x%02X\n", pc, cpu.op_code, busReadAddr(pc + 1), busReadAddr(pc + 2));
                                                                                            
}

/* NOTE: Reads/Writes are 4 T-cycles / 1 M-cycle per byte.*/
/* NOTE2: Any bus reads / writes need to call gbTick, registers do not */
static void cpuGetData(void) {
    cpu.memory_destination = 0x0000;
    cpu.to_memory = false;
    uint8_t upper;
    uint8_t lower;
    uint8_t addr8;
    uint16_t result;
    uint16_t addr16;

    switch(cpu.instruction->addr_mode) {
        case M_NONE:
            return;

        case M_REG:
            cpu.data = cpuReadReg(cpu.instruction->r1);
            return;

        case M_REG_REG:
            cpu.data = cpuReadReg(cpu.instruction->r2);
            return;

        case M_REG_D8:
            cpu.data = busReadAddr(cpu.regs.pc);
            gbTick(1);
            cpu.regs.pc++;
            return;

        case M_MEMREG_REG:
            cpu.data = cpuReadReg(cpu.instruction->r2);
            cpu.memory_destination = cpuReadReg(cpu.instruction->r1);
            cpu.to_memory = true;
            /* Special case */
            if(cpu.instruction->r1 == R_C) {
                cpu.memory_destination |= 0xFF00;
            }
            return;

        case M_REG_MEMREG:
            /* Read the address being passed in (r2) */
            uint16_t addr = cpuReadReg(cpu.instruction->r2);
            /* Special case like above for C register from instr set guide */
            if(cpu.instruction->r2 == R_C) {
                addr |= 0xFF00;
            }
            /* Read from addr */
            cpu.data = busReadAddr(addr);
            gbTick(1);
            return;
        
        /* These do the same thing from a reading standpoint... */
        case M_D16:
        case M_REG_D16:
            lower = busReadAddr(cpu.regs.pc);
            gbTick(1);
            
            upper = busReadAddr(cpu.regs.pc + 1);
            cpu.data = lower | (upper << 8);
            gbTick(1);

            cpu.regs.pc = cpu.regs.pc + 2;
            return;
        
        case M_REG_HLI:
            cpu.data = busReadAddr(cpuReadReg(cpu.instruction->r2));
            gbTick(1);
            cpuWriteReg(R_HL, cpuReadReg(R_HL) + 1);
            return;

        case M_REG_HLD:
            cpu.data = busReadAddr(cpuReadReg(cpu.instruction->r2));
            gbTick(1);
            cpuWriteReg(R_HL, cpuReadReg(R_HL) - 1);
            return;
        
        case M_REG_A8:
            cpu.data = busReadAddr(cpu.regs.pc);
            gbTick(1);
            cpu.regs.pc++;
            return;
            
        case M_HLI_R:
            cpu.data = cpuReadReg(cpu.instruction->r2);
            cpu.memory_destination = cpuReadReg(cpu.instruction->r1);
            cpu.to_memory = true;
            cpuWriteReg(R_HL, cpuReadReg(R_HL) + 1);
            return;

        case M_HLD_R:
            cpu.data = cpuReadReg(cpu.instruction->r2);
            cpu.memory_destination = cpuReadReg(cpu.instruction->r1);
            cpu.to_memory = true;
            cpuWriteReg(R_HL, cpuReadReg(R_HL) - 1);
            return;
        
        case M_A8_REG:
            cpu.memory_destination = busReadAddr(cpu.regs.pc) | 0xFF00;
            gbTick(1);
            cpu.to_memory = true;
            cpu.regs.pc++;
            return;
        
        case M_A16_REG:
            lower = busReadAddr(cpu.regs.pc); // bottom half of address
            gbTick(1);
            upper = busReadAddr(cpu.regs.pc + 1); // top half of address
            gbTick(1);
            
            cpu.to_memory = true;
            cpu.memory_destination = lower | (upper << 8);
            cpu.data = cpuReadReg(cpu.instruction->r2);

            cpu.regs.pc = cpu.regs.pc + 2;
            return;
        
        case M_D16_REG:
            lower = busReadAddr(cpu.regs.pc);
            gbTick(1);
            upper = busReadAddr(cpu.regs.pc+1);
            gbTick(1);

            cpu.to_memory = true;
            cpu.memory_destination = lower | (upper << 8);
            cpu.data = cpuReadReg(cpu.instruction->r2);

            cpu.regs.pc = cpu.regs.pc + 2;
            return;

        case M_REG_A16:
            lower = busReadAddr(cpu.regs.pc);
            gbTick(1);
            
            upper = busReadAddr(cpu.regs.pc + 1);
            gbTick(1);
            
            addr16 = lower | (upper << 8);
            cpu.data = busReadAddr(addr);
            gbTick(1);

            cpu.regs.pc = cpu.regs.pc + 2;
            return;

        case M_HL_SPR:
            cpu.data = busReadAddr(cpu.regs.pc);
            gbTick(1);
            cpu.regs.pc++;
            return;

        case M_D8:
            cpu.data = busReadAddr(cpu.regs.pc);
            gbTick(1);
            cpu.regs.pc++;
            return;
        
        case M_MEMREG:
            cpu.memory_destination = cpuReadReg(cpu.instruction->r1);
            cpu.to_memory = true;
            cpu.data = busReadAddr(cpu.memory_destination);
            gbTick(1);
            return;
        
        case M_MEMREG_D8:
            cpu.data = busReadAddr(cpu.regs.pc);
            gbTick(1);
            cpu.to_memory = true;
            cpu.memory_destination = cpuReadReg(cpu.instruction->r1);
            cpu.regs.pc++;
            return ;
            
        default:
            printf("Address Mode Not Valid\n");
            return;
    }
}

void cpuIntProc(uint16_t addr) {
    /* push current addr to stack
       set pc to interrupt addr      */
    stackPush16(cpu.regs.pc);
    cpu.regs.pc = addr;   
}

void cpuIntHandler() {
    uint32_t i;

    // Cycle through the possible cpu interrupts, return after processing the first one enabled
    for(i = 0; i < CPU_NUM_INT; i++) {
        if(cpu.int_flags & cpu_interrupts[i].type && cpu.ie_reg & cpu_interrupts[i].type) {
            cpuIntProc(cpu_interrupts[i].addr);
            cpu.int_flags &= ~cpu_interrupts[i].type;
            cpu.halted = false;
            cpu.int_enable = false;

            return;
        }
    }
}

static void cpuExec(void) {
    /* Grab asm function & execute */
    ASM_FUNC_PTR p_func = asmGetFunction(cpu.instruction->type);
    if(p_func != NULL) {
        p_func();
    }
    else {
        printf("ASM function not defined: %s\n", instruction_set_s[cpu.instruction->type]);
        exit(-1);
    }
}

bool cpuStep(void) {
    if(!cpu.halted) {
        
        cpuGetInstruction();
        gbTick(1);
        cpuGetData();

        debugUpdate();
        debugShow();

        //printf("0x%04X (0x%04X)\n", cpu.regs.pc, cpu.op_code);
        cpuExec();
    }
    else {
        gbTick(1);
        if(cpu.int_flags > 0) {
            cpu.halted = false;
        }
    }

    // Handle interrupts :D
    if(cpu.int_enable) {
        cpuIntHandler();
        cpu.enabling_ime = false;
    }

    if(cpu.enabling_ime) {
        cpu.int_enable = true;
    }
    
    return true;
}

void cpuInit(void) {
    cpu.regs.pc = 0x0100;
    cpu.regs.sp = 0xFFFE;
    cpu.regs.a = 0x35;
    asmSetCpuPtr(&cpu);
}

void cpuShowInstruction(uint32_t i) {
    printf("INSTRUCTION: %s\n", instruction_set_s[i]);
}

char * cpuGetInsString(uint32_t i) {
    return instruction_set_s[i];
}

char * cpuGetRegString(uint32_t i) {
    return registers_s[i];
}

uint8_t cpuGetIntFlags() {
    return cpu.int_flags;
}
void cpuSetIntFlags(uint8_t val) {
    cpu.int_flags = val;
}

bool cpuGetFlag(uint32_t flag) {
    return BIT_CHECK(cpu.regs.f, flag);
}

uint8_t cpuGetIE(void) {
    return cpu.ie_reg;
}

void cpuSetIE(uint8_t val) {
    cpu.ie_reg = val;
}

void cpuGetFlags(int8_t * z, int8_t * n, int8_t * h, int8_t * c) {
    *z = BIT_CHECK(cpu.regs.f, ZERO_FLAG);
    *n = BIT_CHECK(cpu.regs.f, SUBTRACTION_FLAG);
    *h = BIT_CHECK(cpu.regs.f, HALF_CARRY_FLAG);
    *z = BIT_CHECK(cpu.regs.f, CARRY_FLAG);
}

void cpuSetFlags(CPU_FLAGS flags){
    if(flags.z != -1) {
        BIT_SET(cpu.regs.f, ZERO_FLAG, flags.z);
    }
    if(flags.n != -1) {
        BIT_SET(cpu.regs.f, SUBTRACTION_FLAG, flags.n);
    }
    if(flags.h != -1){
        BIT_SET(cpu.regs.f, HALF_CARRY_FLAG, flags.h);
    }
    if(flags.c != -1){
        BIT_SET(cpu.regs.f, CARRY_FLAG, flags.c);
    }
}

