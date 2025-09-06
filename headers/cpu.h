#include <stdint.h>

typedef enum {
    /* SM83 INSTRUCTION SET */
    O_NONE,
    O_NOP,
    O_LD,
    O_INC,
    O_DEC,
    O_RL,
    O_RLA,
    O_JR,
    O_ADD,
    O_RRCA,
    O_CPL,
    O_CCF,
    O_HALT,
    O_ADC,
    O_SUB,
    O_SBC,
    O_XOR,
    O_OR,
    O_CP,
    O_RET,
    O_POP,
    O_JP,
    O_CALL,
    O_PUSH,
    O_RST,
    O_PREFIX,
    O_EI,
    O_DI,
    O_LDH,
    O_RLCA,
    O_STOP,
    /* CB PREFIX */
    O_RLC,
    O_RRC,
    O_RR,
    O_SLA,
    O_SRA,
    O_SWAP,
    O_SRL,
    O_BIT,
    O_RES,
    O_SET
} CPU_OPCODE_ENUM;

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
    CPU_OPCODE_ENUM op_code;
    CPU_ADDR_MODE_ENUM addr_mode;
    CPU_CONDITION_ENUM condition;
    CPU_REGISTER_ENUM r1;
    CPU_REGISTER_ENUM r2;
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

