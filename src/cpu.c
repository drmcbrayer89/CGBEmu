#include "cpu.h"
#include "bus.h"
#include "gb.h"
#include "asm_func.h"

static CPU cpu;

static CPU_INSTRUCTION instruction_set[0xFFFF] = {
    /* First Row */
    [0x0000] = {I_NOP,  M_NONE                      },
    [0x0001] = {I_LD,   M_REG_D16,      R_BC        },
    [0x0002] = {I_LD,   M_MEMREG_REG,   R_BC,    R_A},
    [0x0003] = {I_INC,  M_REG,          R_BC        },
    [0x0004] = {I_INC,  M_REG,          R_B         },
    [0x0005] = {I_DEC,  M_REG,          R_B         },
    [0x0006] = {I_LD,   M_REG_D8,       R_B         },
    [0x0007] = {I_RLCA                              },
    [0x0008] = {I_LD,   M_A16_REG,      R_SP        },
    [0x0009] = {I_ADD,  M_REG_REG,      R_HL,   R_BC},
    [0x000A] = {I_LD,   M_REG_MEMREG,   R_A,    R_BC},
    [0x000B] = {I_DEC,  M_REG,          R_BC        },
    [0x000C] = {I_INC,  M_REG,          R_C         },
    [0x000D] = {I_DEC,  M_REG,          R_C         },
    [0x000F] = {I_RRCA                              },
    /* Second Row */
    [0x0010] = {I_STOP, M_D8                        },
    [0x0011] = {I_LD,   M_REG_D16,      R_DE        },
    [0x0012] = {I_LD,   M_MEMREG_REG,   R_DE,    R_A},
    [0x0013] = {I_INC,  M_REG,          R_DE        },
    [0x0014] = {I_INC,  M_REG,          R_D         },
    [0x0015] = {I_DEC,  M_REG,          R_D         },
    [0x0016] = {I_LD,   M_REG,          R_D         },
    [0x0017] = {I_RLA                               },
    [0x0018] = {I_JR,   M_D8,                       },
    [0x0019] = {I_ADD,  M_REG_REG,      R_HL,   R_DE},
    [0x001A] = {I_LD,   M_REG_MEMREG,   R_A,    R_DE},
    [0x001B] = {I_DEC,  M_REG,          R_DE        },
    [0x001C] = {I_INC,  M_REG,          R_E         },
    [0x001D] = {I_DEC,  M_REG,          R_E         },
    [0x001E] = {I_LD,   M_REG_D8,       R_E,        },
    [0x001F] = {I_SRA                               },
    /* Third Row */
    [0x0020] = {I_JR, M_D8, R_NONE, R_NONE, C_NZ},
    [0x0021] = {I_LD, M_REG_D16, R_HL},
    [0x0022] = {I_LD, M_HLI_R, R_HL, R_A},
    [0x0023] = {I_INC, M_REG, R_HL},
    [0x0024] = {I_INC, M_REG, R_H},
    [0x0025] = {I_DEC, M_REG, R_H},
    [0x0026] = {I_LD, M_REG_D8, R_H},
    [0x0027] = {I_DAA},
    [0x0028] = {I_JR, M_D8, R_NONE, R_NONE, C_NZ},
    [0x0029] = {I_ADD, M_REG_REG, R_HL, R_HL},
    [0x002A] = {I_LD, M_REG_HLI, R_A, R_HL},
    [0x002B] = {I_DEC, M_REG, R_HL},
    [0x002C] = {I_INC, M_REG, R_L},
    [0x002D] = {I_DEC, M_REG, R_L},
    [0x002E] = {I_LD, M_REG_D8, R_L},
    [0x002F] = {I_CPL},
    /* Fourth Row */
    [0x0030] = {I_JR, M_D8, R_NONE, R_NONE, C_NC},
    [0x0031] = {I_LD, M_REG_D16, R_SP},
    [0x0032] = {I_LD, M_HLD_R, R_HL, R_A},
    [0x0033] = {I_INC, M_REG, R_SP},
    [0x0034] = {I_INC, M_MEMREG, R_HL},
    [0x0035] = {I_DEC, M_MEMREG, R_HL},
    [0x0036] = {I_LD, M_MEMREG_D8, R_HL},
    [0x0037] = {I_SCF},
    [0x0038] = {I_JR, M_D8, R_NONE, R_NONE, C_C},
    [0x0039] = {I_ADD, M_REG_REG, R_HL, R_SP},
    [0x003A] = {I_LD, M_REG_HLD, R_A, R_HL},
    [0x003B] = {I_DEC, M_REG, R_SP},
    [0x003C] = {I_INC, M_REG, R_A},
    [0x003D] = {I_DEC, M_REG, R_A},
    [0x003E] = {I_LD, M_REG_D8, R_A},
    [0x003F] = {I_CCF},
    /* Fifth Row */
    [0x0040] = {I_LD, M_REG_REG, R_B, R_B},
    [0x0041] = {I_LD, M_REG_REG, R_B, R_C},
    [0x0042] = {I_LD, M_REG_REG, R_B, R_D},
    [0x0043] = {I_LD, M_REG_REG, R_B, R_E},
    [0x0044] = {I_LD, M_REG_REG, R_B, R_H},
    [0x0045] = {I_LD, M_REG_REG, R_B, R_L},
    [0x0046] = {I_LD, M_REG_MEMREG, R_B, R_HL},
    [0x0047] = {I_LD, M_REG_REG, R_B, R_A},
    [0x0048] = {I_LD, M_REG_REG, R_C, R_B},
    [0x0049] = {I_LD, M_REG_REG, R_C, R_C},
    [0x004A] = {I_LD, M_REG_REG, R_C, R_D},
    [0x004B] = {I_LD, M_REG_REG, R_C, R_E},
    [0x004C] = {I_LD, M_REG_REG, R_C, R_H},
    [0x004D] = {I_LD, M_REG_REG, R_C, R_L},
    [0x004E] = {I_LD, M_REG_MEMREG, R_C, R_HL},
    [0x004F] = {I_LD, M_REG_REG, R_C, R_A},
    /* Sixth Row */
    [0x0050] = {I_LD, M_REG_REG, R_D, R_B},
    [0x0051] = {I_LD, M_REG_REG, R_D, R_C},
    [0x0052] = {I_LD, M_REG_REG, R_D, R_D},
    [0x0053] = {I_LD, M_REG_REG, R_D, R_E},
    [0x0054] = {I_LD, M_REG_REG, R_D, R_H},
    [0x0055] = {I_LD, M_REG_REG, R_D, R_L},
    [0x0056] = {I_LD, M_REG_MEMREG, R_D, R_HL},
    [0x0057] = {I_LD, M_REG_REG, R_D, R_A},
    [0x0058] = {I_LD, M_REG_REG, R_E, R_B},
    [0x0059] = {I_LD, M_REG_REG, R_E, R_C},
    [0x005A] = {I_LD, M_REG_REG, R_E, R_C},
    [0x005B] = {I_LD, M_REG_REG, R_E, R_E},
    [0x005C] = {I_LD, M_REG_REG, R_E, R_H},
    [0x005D] = {I_LD, M_REG_REG, R_E, R_L},
    [0x005E] = {I_LD, M_REG_MEMREG, R_E, R_HL},
    [0x005F] = {I_LD, M_REG_REG, R_E, R_A},
    /* Seventh Row */
    [0x0060] = {I_LD, M_REG_REG, R_H, R_B},
    [0x0061] = {I_LD, M_REG_REG, R_H, R_C},
    [0x0062] = {I_LD, M_REG_REG, R_H, R_D},
    [0x0063] = {I_LD, M_REG_REG, R_H, R_E},
    [0x0064] = {I_LD, M_REG_REG, R_H, R_H},
    [0x0065] = {I_LD, M_REG_REG, R_H, R_L},
    [0x0066] = {I_LD, M_REG_MEMREG, R_H, R_HL},
    [0x0067] = {I_LD, M_REG_REG, R_H, R_A},
    [0x0068] = {I_LD, M_REG_REG, R_L, R_B},
    [0x0069] = {I_LD, M_REG_REG, R_L, R_C},
    [0x006A] = {I_LD, M_REG_REG, R_L, R_D},
    [0x006B] = {I_LD, M_REG_REG, R_L, R_E},
    [0x006C] = {I_LD, M_REG_REG, R_L, R_H},
    [0x006D] = {I_LD, M_REG_REG, R_L, R_L},
    [0x006E] = {I_LD, M_REG_MEMREG, R_L, R_HL},
    [0x006F] = {I_LD, M_REG_REG, R_L, R_A},
    /* Eighth Row */
    [0x0070] = {I_LD, M_MEMREG_REG, R_HL, R_B},
    [0x0071] = {I_LD, M_MEMREG_REG, R_HL, R_C},
    [0x0072] = {I_LD, M_MEMREG_REG, R_HL, R_D},
    [0x0073] = {I_LD, M_MEMREG_REG, R_HL, R_E},
    [0x0074] = {I_LD, M_MEMREG_REG, R_HL, R_H},
    [0x0075] = {I_LD, M_MEMREG_REG, R_HL, R_L},
    [0x0076] = {I_HALT},
    [0x0077] = {I_LD, M_MEMREG_REG, R_HL, R_A},
    [0x0078] = {I_LD, M_REG_REG, R_A, R_B},
    [0x0079] = {I_LD, M_REG_REG, R_A, R_C},
    [0x007A] = {I_LD, M_REG_REG, R_A, R_D},
    [0x007B] = {I_LD, M_REG_REG, R_A, R_E},
    [0x007C] = {I_LD, M_REG_REG, R_A, R_L},
    [0x007D] = {I_LD, M_REG_REG, R_A, R_L},
    [0x007E] = {I_LD, M_REG_MEMREG, R_A, R_HL},
    [0x007F] = {I_LD, M_REG_REG, R_A, R_A},
    /* NINTH ROW */
    [0x0080] = {I_ADD, M_REG_REG, R_A, R_B},
    [0x0081] = {I_ADD, M_REG_REG, R_A, R_C},
    [0x0082] = {I_ADD, M_REG_REG, R_A, R_D},
    [0x0083] = {I_ADD, M_REG_REG, R_A, R_E},
    [0x0084] = {I_ADD, M_REG_REG, R_A, R_H},
    [0x0085] = {I_ADD, M_REG_REG, R_A, R_L},
    [0x0086] = {I_ADD, M_REG_MEMREG, R_A, R_H},
    [0x0087] = {I_ADD, M_REG_REG, R_A, R_A},
    [0x0088] = {I_ADC, M_REG_REG, R_A, R_B},
    [0x0089] = {I_ADC, M_REG_REG, R_A, R_C},
    [0x008A] = {I_ADC, M_REG_REG, R_A, R_D},
    [0x008B] = {I_ADC, M_REG_REG, R_A, R_E},
    [0x008C] = {I_ADC, M_REG_REG, R_A, R_H},
    [0x008D] = {I_ADC, M_REG_REG, R_A, R_L},
    [0x008E] = {I_ADC, M_REG_REG, R_A, R_HL},
    [0x008F] = {I_ADC, M_REG_REG, R_A, R_A},
    /* TENTH ROW */
    [0x0090] = {I_SUB, M_REG_REG, R_A, R_B},
    [0x0091] = {I_SUB, M_REG_REG, R_A, R_C},
    [0x0092] = {I_SUB, M_REG_REG, R_A, R_D},
    [0x0093] = {I_SUB, M_REG_REG, R_A, R_E},
    [0x0094] = {I_SUB, M_REG_REG, R_A, R_H},
    [0x0095] = {I_SUB, M_REG_REG, R_A, R_L},
    [0x0096] = {I_SUB, M_REG_MEMREG, R_A, R_HL},
    [0x0097] = {I_SUB, M_REG_REG, R_A, R_A},
    [0x0098] = {I_SBC, M_REG_REG, R_A, R_B},
    [0x0099] = {I_SBC, M_REG_REG, R_A, R_C},
    [0x009A] = {I_SBC, M_REG_REG, R_A, R_D},
    [0x009B] = {I_SBC, M_REG_REG, R_A, R_E},
    [0x009C] = {I_SBC, M_REG_REG, R_A, R_H},
    [0x009D] = {I_SBC, M_REG_REG, R_A, R_L},
    [0x009E] = {I_SBC, M_REG_MEMREG, R_A, R_HL},
    [0x009F] = {I_SBC, M_REG_REG, R_A, R_A},
    /* ELEVENTH ROW */
    [0x00A0] = {I_AND, M_REG_REG, R_A, R_B},
    [0x00A1] = {I_AND, M_REG_REG, R_A, R_C},
    [0x00A2] = {I_AND, M_REG_REG, R_A, R_D},
    [0x00A3] = {I_AND, M_REG_REG, R_A, R_E},
    [0x00A4] = {I_AND, M_REG_REG, R_A, R_H},
    [0x00A5] = {I_AND, M_REG_REG, R_A, R_L},
    [0x00A6] = {I_AND, M_REG_MEMREG, R_A, R_HL},
    [0x00A7] = {I_AND, M_REG_REG, R_A, R_A},
    [0x00A8] = {I_XOR, M_REG_REG, R_A, R_B},
    [0x00A9] = {I_XOR, M_REG_REG, R_A, R_C},
    [0x00AA] = {I_XOR, M_REG_REG, R_A, R_D},
    [0x00AB] = {I_XOR, M_REG_REG, R_A, R_E},
    [0x00AC] = {I_XOR, M_REG_REG, R_A, R_H},
    [0x00AD] = {I_XOR, M_REG_REG, R_A, R_L},
    [0x00AE] = {I_XOR, M_REG_MEMREG, R_A, R_HL},
    [0x00AF] = {I_XOR, M_REG_REG, R_A, R_A},
    /* ROW TWELVE */
    [0x00B0] = {I_OR, M_REG_REG, R_A, R_B},
    [0x00B1] = {I_OR, M_REG_REG, R_A, R_C},
    [0x00B2] = {I_OR, M_REG_REG, R_A, R_D},
    [0x00B3] = {I_OR, M_REG_REG, R_A, R_E},
    [0x00B4] = {I_OR, M_REG_REG, R_A, R_H},
    [0x00B5] = {I_OR, M_REG_REG, R_A, R_L},
    [0x00B6] = {I_OR, M_REG_MEMREG, R_A, R_HL},
    [0x00B7] = {I_OR, M_REG_REG, R_A, R_A},
    [0x00B8] = {I_CP, M_REG_REG, R_A, R_B},
    [0x00B9] = {I_CP, M_REG_REG, R_A, R_C},
    [0x00BA] = {I_CP, M_REG_REG, R_A, R_D},
    [0x00BB] = {I_CP, M_REG_REG, R_A, R_E},
    [0x00BC] = {I_CP, M_REG_REG, R_A, R_H},
    [0x00BD] = {I_CP, M_REG_REG, R_A, R_L},
    [0x00BE] = {I_CP, M_REG_MEMREG, R_A, R_HL},
    [0x00BF] = {I_CP, M_REG_REG, R_A, R_A},
    /* ROW THIRTEEN */
    [0x00C0] = {I_RET, M_NONE, R_NONE, R_NONE, C_NZ},
    [0x00C1] = {I_POP, M_REG, R_BC},
    [0x00C2] = {I_JP, M_D16, R_NONE, R_NONE, C_NZ},
    [0x00C3] = {I_JP, M_D16},
    [0x00C4] = {I_CALL, M_D16, R_NONE, R_NONE, C_NZ},
    [0x00C5] = {I_PUSH, M_REG, R_BC},
    [0x00C6] = {I_ADD, M_REG_D8, R_A},
    [0x00C7] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x00},
    [0x00C8] = {I_RET, M_NONE, R_NONE, R_NONE, C_Z},
    [0x00C9] = {I_RET},
    [0x00CA] = {I_JP, M_D16, R_NONE, R_NONE, C_Z},
    [0x00CB] = {I_CB, M_D8},
    [0x00CC] = {I_CALL, M_D16, R_NONE, R_NONE, C_Z},
    [0x00CD] = {I_CALL, M_D16},
    [0x00CE] = {I_ADC, M_REG_D8, R_A},
    [0x00CF] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x08},
    /* ROW FOURTEEN */
    [0x00D0] = {I_RET, M_NONE, R_NONE, R_NONE, C_NC},
    [0x00D1] = {I_POP, M_REG, R_DE},
    [0x00D2] = {I_JP, M_D16, R_NONE, R_NONE, C_NC},
    [0x00D3] = {I_NONE},
    [0x00D4] = {I_CALL, M_D16, R_NONE, R_NONE, C_NC},
    [0x00D5] = {I_PUSH, M_REG, R_DE},
    [0x00D6] = {I_SUB, M_REG_D8, R_A},
    [0x00D7] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x10},
    [0x00D8] = {I_RET, M_NONE, R_NONE, R_NONE, C_C},
    [0x00D9] = {I_RETI},
    [0x00DA] = {I_JP, M_D16, R_NONE, R_NONE, C_C},
    [0x00DB] = {I_NONE},
    [0x00DC] = {I_CALL, M_D16, R_NONE, R_NONE, C_C},
    [0x00DD] = {I_NONE},
    [0x00DE] = {I_SBC, M_REG_D8, R_A},
    [0x00DF] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x18},
    /* ROW FIFTEEN */
    [0X00E0] = {I_LDH, M_A8_REG, R_NONE, R_A},
    [0X00E1] = {I_POP, M_REG, R_HL},
    [0X00E2] = {I_LD, M_MEMREG_REG, R_C, R_A},
    [0X00E5] = {I_PUSH, M_REG, R_HL},
    [0X00E6] = {I_AND, M_REG_D8, R_SP},
    [0X00E7] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x20},
    [0X00E8] = {I_ADD, M_REG_D8, R_SP},
    [0X00E9] = {I_JP, M_REG, R_HL},
    [0X00EA] = {I_LD, M_A16_REG, R_NONE, R_A},
    [0X00EE] = {I_XOR, M_REG_D8, R_A},
    [0X00EF] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x28},
    /* ROW SIXTEEN */
    [0X00F0] = {I_LDH, M_REG_A8, R_A},
    [0X00F1] = {I_POP, M_REG, R_AF},
    [0X00F2] = {I_LD, M_REG_MEMREG, R_A, R_C},
    [0X00F3] = {I_DI},
    [0X00F5] = {I_PUSH, M_REG, R_AF},
    [0X00F6] = {I_OR, M_REG_D8, R_A},
    [0X00F7] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x30},
    [0X00F8] = {I_LD, M_HL_SPR, R_HL, R_SP},
    [0X00F9] = {I_LD, M_REG_REG, R_SP, R_HL},
    [0X00FA] = {I_LD, M_REG_A16, R_A},
    [0X00FB] = {I_EI},
    [0X00FE] = {I_CP, M_REG_D8, R_A},
    [0X00FF] = {I_RST, M_NONE, R_NONE, R_NONE, C_NONE, 0x38}
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
            printf("Register enumeration %i not found\n", reg);
            return 0;
    }
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
        default:
            printf("Incorrect register!\n");
            return;
    }
}

static void cpuGetInstruction(void) {
    uint16_t pc = cpu.regs.pc;
    cpu.op_code = busReadAddr(cpu.regs.pc++);
    cpu.instruction = cpuGetInstructionByOpCode(cpu.op_code);
    printf("PC: 0x%04X %-4s 0x%02X 0x%02X\n",   pc, 
                                            instruction_set_s[cpu.instruction->type], 
                                            busReadAddr(cpu.regs.pc + 1), 
                                            busReadAddr(cpu.regs.pc + 2));
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
            /* Read from addr into r1 */
            cpu.data = cpuReadReg(cpu.instruction->r1);
            return;
        
        /* These do the same thing from a reading standpoint... */
        case M_D16:
        case M_REG_D16:
            lower = busReadAddr(cpu.regs.pc);
            gbTick();
            
            upper = busReadAddr(cpu.regs.pc + 1);
            cpu.data = lower | (upper << 8);
            gbTick();

            cpu.regs.pc = cpu.regs.pc + 2;
            return;
        
        case M_REG_HLI:
            cpu.data = busReadAddr(cpuReadReg(cpu.instruction->r2));
            gbTick();
            cpuWriteReg(R_HL, cpuReadReg(R_HL) + 1);
            return;

        case M_REG_HLD:
            cpu.data = busReadAddr(cpuReadReg(cpu.instruction->r2));
            gbTick();
            cpuWriteReg(R_HL, cpuReadReg(R_HL) - 1);
            return;
        
        case M_REG_A8:
            cpu.data = busReadAddr(cpu.regs.pc++);
            gbTick();
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
            gbTick();
            cpu.to_memory = true;
            cpu.regs.pc++;
            return;
        
        case M_A16_REG:
            lower = busReadAddr(cpu.regs.pc); // bottom half of address
            gbTick();
            upper = busReadAddr(cpu.regs.pc + 1); // top half of address
            gbTick();
            
            cpu.to_memory = true;
            cpu.memory_destination = lower | (upper << 8);
            cpu.data = cpuReadReg(cpu.instruction->r2);

            cpu.regs.pc = cpu.regs.pc + 2;
            return;
        
        case M_D16_REG:
            lower = busReadAddr(cpu.regs.pc);
            gbTick();
            upper = busReadAddr(cpu.regs.pc+1);
            gbTick();

            cpu.to_memory = true;
            cpu.memory_destination = lower | (upper << 8);
            cpu.data = cpuReadReg(cpu.instruction->r2);

            cpu.regs.pc = cpu.regs.pc + 2;
            return;

        case M_REG_A16:
            lower = busReadAddr(cpu.regs.pc);
            gbTick();
            
            upper = busReadAddr(cpu.regs.pc + 1);
            gbTick();
            
            addr16 = lower | (upper << 8);
            cpu.data = busReadAddr(addr);
            gbTick();

            cpu.regs.pc = cpu.regs.pc + 2;
            return;

        case M_HL_SPR:
            cpu.data = busReadAddr(cpu.regs.pc);
            gbTick();
            cpu.regs.pc++;
            return;

        case M_D8:
            cpu.data = busReadAddr(cpu.regs.pc);
            gbTick();
            cpu.regs.pc++;
            return;
        
        case M_MEMREG:
            cpu.memory_destination = cpuReadReg(cpu.instruction->r1);
            cpu.to_memory = true;
            cpu.data = busReadAddr(cpu.memory_destination);
            gbTick();
            return;
        
        case M_MEMREG_D8:
            cpu.data = busReadAddr(cpu.regs.pc);
            gbTick();
            cpu.to_memory = true;
            cpu.memory_destination = cpuReadReg(cpu.instruction->r1);
            cpu.regs.pc++;
            return ;
            
        default:
            printf("Address Mode Not Valid\n");
            return;
    }
}

static void cpuExec(void) {
    /* Grab asm function & execute -- pass pointer to CPU */
    ASM_FUNC_PTR p_func = asmGetFunction(cpu.instruction->type);
    if(p_func != NULL) {
        p_func(&cpu);
    }
    else {
        printf("ASM function not defined\n");
        exit(-1);
    }
}

bool cpuStep(void) {
    if(cpu.halted == false) {
        cpuGetInstruction();
        cpuGetData();
        cpuExec();
        return true;
    }

    return false;
}

void cpuInit(void) {
    cpu.regs.pc = 0x0100;
}

void cpuShowInstruction(uint32_t i) {
    printf("INSTRUCTION: %s\n", instruction_set_s[i]);
}
