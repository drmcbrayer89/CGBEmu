#include "cpu.h"
#include "bus.h"

static CPU_INSTRUCTION instructions[0xFFFF] = {
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
    [0x003F] = {I_CCF}
    /* Fifth Row */

};

CPU_INSTRUCTION * cpuGetInstructionByOpCode(uint16_t op_code) {
    return &instructions[op_code];
}
