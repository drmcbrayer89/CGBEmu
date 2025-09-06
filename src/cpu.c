#include "cpu.h"
#include "bus.h"

CPU_INSTRUCTION instructions[0xFFFF] = {
    [0x0000] = {OP_NOP, M_NONE},
    [0x0001] = {OP_LD, M_REG_D16},
    [0x0002] = {OP_LD, M_MEMREG_REG},
    [0x0003] = {OP_INC, M_REG},
    [0x0004] = {OP_INC, M_REG},
    [0x0005] = {OP_DEC, M_REG},
    [0x0006] = {OP_LD, M_REG_D8},
};
