#include "timer.h"

static TIMER timer = {0};

void timerWrite(uint16_t addr, uint8_t val) {
    //TODO read pan docs or other stuff later
    switch(addr) {
        case 0xFF04:
            timer.div = 0x00;
            return;
        case 0xFF05:
            timer.tima = val;
            return;
        case 0xFF06:
            timer.tma = val;
        case 0xFF07:
            timer.tac = val;
            return;
        default:
            printf("Incorrect timer address\n");
            return;
    }
}
uint16_t timerRead(uint16_t addr) {
    return 0;
}