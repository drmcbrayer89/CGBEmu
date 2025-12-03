#include "common.h"

#define KBYTE 1024

typedef struct {
    uint8_t wram[8 * KBYTE];
    uint8_t hram[128];
} MEMORY;

uint8_t memReadWRam(uint16_t addr);
void memWriteWRam(uint16_t addr, uint8_t val);
uint8_t memReadHRam(uint16_t addr);
void memWriteHRam(uint16_t addr, uint8_t val);
