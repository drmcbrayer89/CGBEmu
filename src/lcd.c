#include "lcd.h"
/* 
    FF41 LCD Status:
        b7 - none
        b6 - lyc int sel 
        b5 - mode 2 int sel
        b4 - mode 1 int sel
        b3 - mode 0 int sel
        b2 - lyc == ly
        b1 & b0 - PPU mode 
*/

static LCD lcd;

uint8_t lcdGetPpuMode(void) {
    return (lcd.lcds & LCD_PPU_MODE_MASK);
}

void lcdSetPpuMode(uint8_t mode) {
    // Clear current mode
    lcd.lcds &= ~LCD_PPU_MODE_MASK;
    // Set updated mode
    lcd.lcds |= mode;
}

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

LCD * lcdGetPtr(void) {
    return &lcd;
}