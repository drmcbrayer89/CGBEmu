#include "common.h"

typedef void (*ASM_FUNC_PTR)(void);
ASM_FUNC_PTR asmGetFunction(CPU_INSTRUCTION_ENUM i);