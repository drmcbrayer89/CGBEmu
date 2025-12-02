#include "ppu.h"

PPU ppu = {0};

void ppuWriteVram(uint16_t addr, uint8_t val) {
    addr = addr - 0x8000;
    ppu.vram[addr] = val;
}

uint8_t ppuReadVram(uint16_t addr) {
    addr = addr - 0x8000;
    return ppu.vram[addr];
}