#include "cpu.h"
#include "bus.h"

CPU_INSTRUCTION instructions[0xFFFF] = {
    [0x0000] = {O_NOP,  M_NONE                      },
    [0x0001] = {O_LD,   M_REG_D16,      R_BC        },
    [0x0002] = {O_LD,   M_MEMREG_REG,   R_BC,    R_A},
    [0x0003] = {O_INC,  M_REG,          R_BC        },
    [0x0004] = {O_INC,  M_REG,          R_B         },
    [0x0005] = {O_DEC,  M_REG,          R_B         },
    [0x0006] = {O_LD,   M_REG_D8,       R_B         },
    [0x0007] = {O_RCLA                              },
    [0x0008] = {O_LD,   M_A16_REG,      R_SP        },
    [0x0009] = {O_ADD,  M_REG_REG,      R_HL,   R_BC},
    [0x000A] = {O_LD,   M_REG_MEMREG,   R_A,    R_BC},
    [0x000B] = {O_DEC,  M_REG,          R_BC        },
    [0x000C] = {O_INC,  M_REG,          R_C         },
    [0x000D] = {O_DEC,  M_REG,          R_C         },
    [0x000F] = {O_RRCA                              },

    [0x0010] = {O_STOP, M_D8                        },
    [0x0011] = {O_LD,   M_REG_D16,      R_DE        },
    [0x0012] = {O_LD,   M_MEMREG_REG,   R_DE,    R_A},
    [0x0013] = {O_INC,  M_REG,          R_DE        },
    [0x0014] = {O_INC,  M_REG,          R_D         },
    [0x0015] = {O_DEC,  M_REG,          R_D         },
    [0x0016] = {O_LD,   M_REG,          R_D         },
    [0x0017] = {O_RLA                               },
    [0x0018] = {O_JR,   M_D8,                       },
    [0x0019] = {O_ADD,  M_REG_REG,      R_HL,   R_DE},
    [0x001A] = {O_LD,   M_REG_MEMREG,   R_A,    R_DE},
    [0x001B] = {O_DEC,  M_REG,          R_DE        },
    [0x001C] = {O_INC,  M_REG,          R_E         },
    [0x001D] = {O_DEC,  M_REG,          R_E         },
    [0x001E] = {O_LD,   M_REG_D8,       R_E,        },
    [0x001F] = {O_RRA                               }
};
