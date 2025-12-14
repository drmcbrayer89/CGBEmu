#include "common.h"

#define LYC_INT_SELECT 6
#define MODE_2_INT_SELECT 5
#define MODE_1_INT_SELECT 4
#define MODE_0_INT_SELECT 3
#define LYC_EQUALS_LY 2
#define PPU_MODE 1

typedef struct {
    uint8_t lcdc;
    uint8_t ly;
    uint8_t lyc;
    uint8_t dma_enable;
} LCD;

void lcdWrite(uint16_t addr, uint8_t val);
uint8_t lcdRead(uint16_t addr);
