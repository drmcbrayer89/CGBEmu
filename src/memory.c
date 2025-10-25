#include "common.h"
#include "memory.h"

static MEMORY mem[0xFFFF];

uint8_t memReadWRam(uint16_t addr) {
    printf("nothing here\n");
}

void memWriteWRam(uint16_t addr, uint16_t val) {
    printf("nothing here either\n");
}

uint8_t memReadHRam(uint16_t addr) {
    printf("nothing here either\n");
}

void memWriteHRam(uint16_t addr, uint16_t val) {
    printf("nothing here either\n");
}