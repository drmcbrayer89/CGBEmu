#include "common.h"
#include "cpu.h"

typedef void (*ASM_FUNC_PTR)(CPU * p_cpu);
ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i);
void asmSetCpuPtr(CPU * cpu);