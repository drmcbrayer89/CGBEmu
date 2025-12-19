#include "lcd.h"

static LCD lcd;

void lcdWrite(uint16_t addr, uint8_t val) {
    if(addr == 0xFF40) {
        lcd.lcdc = val;
    }
}

uint8_t lcdRead(uint16_t addr) {
    switch(addr) {
        case 0xFF41:
            break;
        case 0xFF44:
            break;
        case 0xFF45:
            break;
        default:
            return 0;
            break;
    }
    return 0;
}