#include "common.h"
#include "memory.h"

static MEMORY mem = {0};

uint8_t memReadWRam(uint16_t addr) {
    addr = addr - 0xC000;
    return mem.wram[addr];
}

void memWriteWRam(uint16_t addr, uint8_t val) {
    addr = addr - 0xC000;
    mem.wram[addr] = val;
}

uint8_t memReadHRam(uint16_t addr) {
    addr = addr - 0xFF80;
    return mem.hram[addr];
}

void memWriteHRam(uint16_t addr, uint8_t val) {
    addr = addr - 0xFF80;
    mem.hram[addr] = val;
}