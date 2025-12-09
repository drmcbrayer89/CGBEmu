#include "common.h"

typedef struct {
    uint8_t vram[0x1FFF];
    uint8_t oam[0x9F];
} PPU;


void ppuWriteOam(uint16_t addr, uint8_t val);
uint8_t ppuReadOam(uint16_t addr);
void ppuWriteVram(uint16_t addr, uint8_t val);
uint8_t ppuReadVram(uint16_t addr);
void ppuInit(void);