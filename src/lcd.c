#include "lcd.h"

static LCD lcd;

void lcdWrite(uint16_t addr, uint8_t val) {
    /* Write LCD shit here */
    switch(addr) {
        case 0xFF40: // LCD Control (LCDC)
            lcd.lcdc = val;
            return;
        case 0xFF41: // LCD Status (LCDS)
            lcd.lcds = val;
            return;
        case 0xFF44: // LY:LCD Y Coordinate
        case 0xFF45: // LYC: LY Compare
            if(lcd.lcds & (1 << 2)) {
                // request STAT interrupt
            }
    }
}

uint8_t lcdRead(uint16_t addr) {
    return 0;
}