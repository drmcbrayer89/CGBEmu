#include "common.h"

#define KBYTE 1024

//typedef uint8_t MEMORY;
typedef struct {
    uint8_t wram[0x1FFF];
    uint8_t hram[0x73];
} MEMORY;

uint8_t memReadWRam(uint16_t addr);
void memWriteWRam(uint16_t addr, uint8_t val);
uint8_t memReadHRam(uint16_t addr);
void memWriteHRam(uint16_t addr, uint8_t val);
