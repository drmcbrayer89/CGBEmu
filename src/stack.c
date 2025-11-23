#include "stack.h"
#include "cpu.h"
#include "bus.h"

void stackPush8(uint8_t val) {
    /*
    uint16_t sp = cpuReadReg(R_SP);
    sp--;
    busWriteAddr(sp, val);
    cpuWriteReg(R_SP, sp);
    */
   uint16_t sp = cpuReadReg(R_SP);
   busWriteAddr16(--sp, val);
   cpuWriteReg(R_SP, val);
}

void stackPush16(uint16_t val) {
    uint8_t hi = (val >> 8) & 0xFF;
    uint8_t lo = val & 0xFF;
    stackPush8(hi);
    stackPush8(lo);
}
uint8_t stackPop8(void) {
    uint16_t sp = cpuReadReg(R_SP);
    uint8_t val = busReadAddr(sp);
    cpuWriteReg(R_SP, ++sp);
    return val;
}
uint16_t stackPop16(void) {
    uint16_t hi = stackPop8();
    uint16_t lo = stackPop8();
    uint16_t result = (hi << 8) | lo;
    return result;
}