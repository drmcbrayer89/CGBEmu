#pragma once
#include "common.h"
/*
    Zero flag is set iff result of operation is zero, used by conditional jumps
    Carry flags are used if values exceed FF or FFFF or when subtraction is lower than 0 or when 
    shifting/rotating out a "1" bit 
*/
#define ZERO_FLAG 7
#define SUBTRACTION_FLAG 6
#define HALF_CARRY_FLAG 5
#define CARRY_FLAG 4

typedef enum {
    I_CB_BIT    = 1,
    I_CB_RES    = 2,
    I_CB_SET    = 3,
    I_CB_RLC    = 0,
    I_CB_RRC    = 1,
    I_CB_RL     = 2,
    I_CB_RR     = 3,
    I_CB_SLA    = 4,
    I_CB_SRA    = 5,
    I_CB_SWAP   = 6,
    I_CB_SRL    = 7
} CPU_INSTRUCTION_CB_ENUM;

typedef enum {
    /* SM83 INSTRUCTION SET */
    I_NONE,
    I_NOP,
    I_LD,
    I_INC,
    I_DEC,
    I_RL,
    I_RLA,
    I_JR,
    I_ADD,
    I_RRCA,
    I_CPL,
    I_CCF,
    I_HALT,
    I_ADC,
    I_SUB,
    I_SBC,
    I_XOR,
    I_OR,
    I_CP,
    I_RET,
    I_POP,
    I_JP,
    I_CALL,
    I_PUSH,
    I_RST,
    I_PREFIX,
    I_EI,
    I_DI,
    I_LDH,
    I_RLCA,
    I_DAA,
    I_STOP,
    I_AND,
    I_CB,
    I_RETI,
    I_RRA,
    /* CB PREFIX */
    I_RLC,
    I_RRC,
    I_RR,
    I_SLA,
    I_SRA,
    I_SWAP,
    I_SRL,
    I_SCF,
    I_BIT,
    I_RES,
    I_SET,
    I_SET_SIZE
} CPU_INSTRUCTION_ENUM;

typedef enum {
    M_NONE,//
    M_REG_D16,//
    M_REG_REG,//
    M_MEMREG_REG,//
    M_REG,//
    M_REG_D8,//
    M_REG_MEMREG,//
    M_REG_HLI,//
    M_REG_HLD,//
    M_HLI_R,//
    M_HLD_R,//
    M_REG_A8,//
    M_A8_REG,//
    M_HL_SPR,//
    M_D16,//
    M_D8,//
    M_D16_REG,//
    M_MEMREG_D8,//
    M_MEMREG,//
    M_A16_REG,//
    M_REG_A16//
} CPU_ADDR_MODE_ENUM;

typedef enum {
    C_NONE,
    C_Z,
    C_C,
    C_NZ,
    C_NC
} CPU_CONDITION_ENUM;

typedef enum {
    R_NONE,
    R_A,
    R_F,
    R_B,
    R_C,
    R_D,
    R_E,
    R_H,
    R_L,
    R_AF,
    R_BC,
    R_DE,
    R_HL,
    R_SP,
    R_PC
} CPU_REGISTER_ENUM;

typedef struct {
    CPU_INSTRUCTION_ENUM type;
    CPU_ADDR_MODE_ENUM addr_mode;
    CPU_REGISTER_ENUM r1;
    CPU_REGISTER_ENUM r2;
    CPU_CONDITION_ENUM condition;
    uint8_t parameter;
} CPU_INSTRUCTION;

typedef struct {
    uint8_t a;
    uint8_t f;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp; // Stack ptr 
    uint16_t pc; // Program Counter
} CPU_REGISTERS;

typedef struct {
    CPU_REGISTERS regs;
    CPU_INSTRUCTION * instruction;
    uint8_t  op_code;
    
    uint16_t data;
    uint16_t memory_destination;
    bool to_memory;

    bool halted;
    bool stepping;
    bool int_enable;
    bool enabling_ime;
    uint8_t int_flags;
    uint8_t ie_reg;
} CPU;

typedef struct {
    int8_t z;
    int8_t n;
    int8_t h;
    int8_t c;
} CPU_FLAGS;

typedef enum {
    INT_VBLANK = 1,
    INT_LCD_STAT = 2,
    INT_TIMER = 4,
    INT_SERIAL = 8,
    INT_JOYPAD = 16
} CPU_INTERRUPT_TYPES;

typedef struct {
    CPU_INTERRUPT_TYPES type;
    uint16_t addr;
} CPU_INTERRUPTS;

CPU_INSTRUCTION * cpuGetInstructionByOpCode(uint16_t op_code);
void cpuGetFlags(int8_t * z, int8_t * n, int8_t * h, int8_t * c);
void cpuSetFlags(CPU_FLAGS flags);
bool cpuGetFlag(uint32_t flag);
uint8_t cpuReadRegCb(CPU_REGISTER_ENUM reg);
void cpuWriteRegCb(CPU_REGISTER_ENUM reg, uint8_t val);
uint16_t cpuReadReg(CPU_REGISTER_ENUM reg);
void cpuWriteReg(CPU_REGISTER_ENUM reg, uint16_t val);
void cpuInit(void);
void cpuShowInstruction(uint32_t i);
char * cpuGetInsString(uint32_t i);
char * cpuGetRegString(uint32_t i);
bool cpuStep(void);
uint8_t cpuGetIE(void);
void cpuSetIE(uint8_t val);
uint8_t cpuGetIntFlags();
void cpuSetIntFlags(uint8_t val);
uint16_t cpuSwapEndian(uint16_t val);