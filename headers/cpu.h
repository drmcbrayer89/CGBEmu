#include <stdint.h>

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
    I_SET
} CPU_INSTRUCTION_ENUM;

typedef enum {
    M_NONE,
    M_REG_D16,
    M_REG_REG,
    M_MEMREG_REG,
    M_REG,
    M_REG_D8,
    M_REG_MEMREG,
    M_REG_HLI,
    M_REG_HLD,
    M_HLI_R,
    M_HLD_R,
    M_REG_A8,
    M_A8_REG,
    M_HL_SPR,
    M_D16,
    M_D8,
    M_D16_REG,
    M_MEMREG_D8,
    M_MEMREG,
    M_A16_REG,
    M_REG_A10
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
    CPU_INSTRUCTION_ENUM op_code;
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

    uint16_t data;
    uint16_t mem_dest;
    uint8_t  op_code;

    bool halted;
    bool stepping;
} CPU;

