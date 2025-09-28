#include "common.h"

typedef struct {
    uint8_t z;
    uint8_t n;
    uint8_t h;
    uint8_t c;
} ASM_FLAGS;

typedef void (*ASM_FUNC_PTR)(void);
ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i);
void asmSetCpuPtr(CPU * cpu);