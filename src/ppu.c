#include "ppu.h"
#include "lcd.h"

#define WIDTH_PX 160
#define MAX_SCANLINES 144
#define DOTS_PER_LINE 456

PPU ppu = {0};

void ppuStateMachine(void) {

}

// Just get this cycling for now.
void ppuTick(void) {
    static uint32_t dots = 0;
    ppu.ticks++;

    dots++;
    switch (ppu.mode) {
        case OAM_SEARCH:
            if(dots == 80) ppu.mode = DRAWING;
            break;
        case DRAWING:
            // This is where I'll grab pixel data from the FIFO
            // & place on the screen. 
            // Just incrementing for now.
            if(++ppu.x == WIDTH_PX) ppu.mode = HBLANK;
            break;
        case HBLANK:
            // Update LY register and transistion to either OAM or VBLANK
            if(dots == DOTS_PER_LINE) {
                dots = 0;
                if(++ppu.ly == MAX_SCANLINES) {
                    ppu.mode = VBLANK;
                }
                else {
                    ppu.mode = OAM_SEARCH;
                }
            }
            break;
        case VBLANK:
            // VBLANK waits for 10 increments of LY...
            if(dots == DOTS_PER_LINE) {
                dots = 0;
                if(++ppu.ly == 153) {
                    ppu.ly = 0;
                    ppu.mode = OAM_SEARCH;
                }
            }
            break;
        default:
            break;
    }
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
    //ppu.oam[addr] = val;
}

uint8_t ppuReadOam(uint16_t addr) {
    addr = addr - OAM_START;
    // Find OAM byte with modulo (Need to test this)
    uint8_t byte_index = addr % 4;
    // Recenter at 0-th byte of OAM entry
    addr = addr - byte_index;

    switch(byte_index) {
        case 0:
            return ppu.oam[addr].y;
        case 1:
            return ppu.oam[addr].x;
        case 2:
            return ppu.oam[addr].tile_index;
        case 3:
            return ppu.oam[addr].attributes.word;
        default:
            return 0;
    }
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
    ppu.ticks = 0;
    ppu.mode = OAM_SEARCH;
}