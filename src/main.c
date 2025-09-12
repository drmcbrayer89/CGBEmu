#include "cart.h"
#include "cpu.h"

int32_t main(int32_t argc, char * argv[])
{
    if(argc > 1){
        printf("Starting CGBEmu!\n");
        cartOpen(argv[1]);
    }

    cpuInit();
    cpuRegRead(R_AF);
    CPU_INSTRUCTION * ins = cpuGetInstructionByOpCode(0x0001);
    cpuShowInstruction(ins->type);
    return 0;
}