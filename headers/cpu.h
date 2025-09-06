#include <stdint.h>

typedef enum {
    /* SM83 INSTRUCTION SET */
    OP_NONE,
    OP_NOP,
    OP_LD,
    OP_INC,
    OP_DEC,
    OP_RL,
    OP_RLA,
    OP_JR,
    OP_ADD,
    OP_RRCA,
    OP_CPL,
    OP_CCF,
    OP_HALT,
    OP_ADC,
    OP_SUB,
    OP_SBC,
    OP_XOR,
    OP_OR,
    OP_CP,
    OP_RET,
    OP_POP,
    OP_JP,
    OP_CALL,
    OP_PUSH,
    OP_RST,
    OP_PREFIX,
    OP_EI,
    OP_DI,
    OP_LDH,

    /* CB PREFIX */
    OP_RLC,
    OP_RRC,
    OP_RR,
    OP_SLA,
    OP_SRA,
    OP_SWAP,
    OP_SRL,
    OP_BIT,
    OP_RES,
    OP_SET
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
    CON_NONE,
    CON_Z,
    CON_C,
    CON_NZ,
    CON_NC
} CPU_INSTRUCTION_CONDITION_TYPE_ENUM;

typedef struct {
    CPU_OPCODE_ENUM op_code;
    CPU_ADDR_MODE_ENUM addr_mode;
    CPU_INSTRUCTION_CONDITION_TYPE_ENUM condition;
    uint8_t r1;
    uint8_t r2;
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

