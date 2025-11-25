#include "stack.h"
#include "cpu.h"
#include "bus.h"

void stackPush8(uint8_t val) {
    uint16_t sp = cpuReadReg(R_SP);
    busWriteAddr(sp - 1, val);
    cpuWriteReg(R_SP, (sp - 1));
    return;
}

void stackPush16(uint16_t val) {
    stackPush8((val >> 8) & 0xFF);
    stackPush8((val & 0xFF));
    return;
}
uint8_t stackPop8(void) {
    uint16_t sp = cpuReadReg(R_SP);
    uint8_t val = busReadAddr(sp);
    cpuWriteReg(R_SP, (sp + 1));
    return val;
}
uint16_t stackPop16(void) {
    uint16_t lo = stackPop8();
    uint16_t hi = stackPop8();
    uint16_t val = (hi << 8) | lo;
    return val;
}