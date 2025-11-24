#include "stack.h"
#include "cpu.h"
#include "bus.h"

void stackPush8(uint8_t val) {
    //printf("PRE PUSH! SP: 0x%04X\n", cpuReadReg(R_SP));

    uint16_t sp = cpuReadReg(R_SP);
    busWriteAddr(sp - 1, val);
    cpuWriteReg(R_SP, (sp - 1));
    //printf("POST PUSH! SP: 0x%04X\n", cpuReadReg(R_SP));
    
    return;
}

void stackPush16(uint16_t val) {
    stackPush8((val >> 8) & 0xFF);
    stackPush8((val & 0xFF));
    return;
}
uint8_t stackPop8(void) {
    //printf("PRE POP! SP: 0x%04X\n", cpuReadReg(R_SP));
    uint16_t sp = cpuReadReg(R_SP);
    uint8_t val = busReadAddr(sp + 1);
    cpuWriteReg(R_SP, (sp + 1));
    //printf("POST POP! SP: 0x%04X\n", cpuReadReg(R_SP));
    
    return val;
}
uint16_t stackPop16(void) {
    //printf("PRE POP! SP: 0x%04X\n", cpuReadReg(R_SP));
    uint16_t lo = stackPop8();
    uint16_t hi = stackPop8();
    //printf("POST POP! SP: 0x%04X\n", cpuReadReg(R_SP));
    uint16_t val = (hi << 8) | lo;
    return val;
}