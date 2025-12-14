#include "ppu.h"
#include "lcd.h"

PPU ppu = {0};

void ppuStateMachine(void) {
    switch(ppu.mode) {
        case MODE_0:
            break;
        case MODE_1:
            break;
        case MODE_2:
            if(ppu.dots <= 80) {
                // OAM scan
                ppu.oam_locked = true;
            }
            else {
                ppu.mode = MODE_3;
                ppu.oam_locked = true;
            }
            break;
        case MODE_3:
            if(ppu.dots <= 289) {
                ppu.vram_locked = true;
            }
            else {
                ppu.oam_locked = false;
            }
            break;
        default: 
            break;
    }
    // Clear after 456 always
    if(ppu.dots == 456) ppu.dots = 0;
}

void ppuTick(void) {
    ppuStateMachine();
    ppu.dots++;
}

void ppuGetColorIndexes(uint16_t line, uint8_t * color_id_out) {
    uint8_t bit_index = 15;
    /* This gets an entire line worth of pixel color id's */
    for(bit_index; bit_index >= 0; bit_index--) {
        uint8_t b1_msb = (line & (1 << bit_index) >> bit_index);
        uint8_t b2_msb = (line & (1 << bit_index - 8) >> (bit_index - 8));
        color_id_out[bit_index] = (b2_msb << 1) | b1_msb;
    }
}

void ppuWriteOam(uint16_t addr, uint8_t val) {
    addr = addr - OAM_START;
    ppu.oam[addr] = val;
}

uint8_t ppuReadOam(uint16_t addr) {
    addr = addr - OAM_START;
    return ppu.oam[addr];
}

void ppuWriteVram(uint16_t addr, uint8_t val) {
    addr = addr - VRAM_START;
    ppu.vram[addr] = val;
}

uint8_t ppuReadVram(uint16_t addr) {
    addr = addr - VRAM_START;
    return ppu.vram[addr];
}

void ppuInit(void) {
    ppu.oam_locked = true;
    ppu.dots = 0;
}