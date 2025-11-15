#include "common.h"
#include "memory.h"

static MEMORY mem[0xFFFF];

uint8_t memReadWRam(uint16_t addr) {
    printf("0x%02X", mem[addr]);
    return mem[addr];
}

void memWriteWRam(uint16_t addr, uint8_t val) {
    printf("WRITING 0x%02X\n", val);
    mem[addr] = val;
}

uint8_t memReadHRam(uint16_t addr) {
    return mem[addr];
}

void memWriteHRam(uint16_t addr, uint8_t val) {
    printf("WRITING 0x%02X\n", val);
    mem[addr] = val;
}