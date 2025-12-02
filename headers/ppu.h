#include "common.h"

typedef struct {
    uint8_t vram[0x1FFF];
} PPU;

void ppuWriteVram(uint16_t addr, uint8_t val);
uint8_t ppuReadVram(uint16_t addr);
void ppuInit(void);