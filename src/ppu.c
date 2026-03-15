#include "ppu.h"

#define WIDTH_PX 160
#define MAX_SCANLINES 144
#define DOTS_PER_LINE 456

static PPU ppu = {0};
static LCD * p_lcd;

// Just get this cycling for now.
void ppuTick(void) {
    static uint32_t dots = 0;
    ppu.ticks++;
    dots++;

    ppu.mode = (PPU_MODE)lcdGetPpuMode();
    switch (ppu.mode) {
        case MODE_OAM:
            break;
        case MODE_DRAW:
            // This is where I'll grab pixel data from the FIFO
            // & place on the screen. 
            // Just incrementing for now.
            break;
        case MODE_HBLANK:
            // Update LY register and transistion to either OAM or VBLANK
            break;
        case MODE_VBLANK:
            // VBLANK waits for 10 increments of LY...
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
    if(addr >= OAM_START) {
        addr = addr - OAM_START;
    }
    ppu.oam_bytes[addr] = val;
}

uint8_t ppuReadOam(uint16_t addr) {
    if(addr >= OAM_START) {
        addr = addr - OAM_START;
    }
    return ppu.oam_bytes[addr];
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
    ppu.mode = MODE_OAM;
    //Easy access to LCD
    p_lcd = lcdGetPtr();
}