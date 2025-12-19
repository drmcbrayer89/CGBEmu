#include "ppu.h"
#include "lcd.h"

PPU ppu = {0};

void ppuStateMachine(void) {

}

// Just get this cycling for now.
void ppuTick(void) {
    ppu.dots++;
    switch (ppu.mode) {
        case OAM_SEARCH:
            ppu.mode = (ppu.dots == 80) ? DRAWING : OAM_SEARCH;
            break;
        case DRAWING:
            ppu.mode = (ppu.dots == (289 + 80)) ? HBLANK : DRAWING;
            break;
        case HBLANK:
            if(ppu.dots == (289 + 80 + 204)) {
                if(ppu.ly++ == 145) {
                    ppu.mode = VBLANK;
                }
            }
            break;
        case VBLANK:
            if(ppu.dots == 289 + 80 + 204) {
                ppu.ly++;
            }
            if(ppu.ly == 154) {
                ppu.ly = 0;
            }
            break;
        default:
            break;
            
    }
    if(ppu.dots == 80) ppu.mode = DRAWING;

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
    ppu.mode = OAM_SEARCH;
}