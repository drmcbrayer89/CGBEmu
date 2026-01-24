#include "common.h"

#define LCDC_SET(a, b) a & (1 << b);

typedef enum {
    BG_WINDOW_ENABLE = 0,
    OBJ_ENABLE,
    OBJ_SIZE,
    BG_TILE_MAP,
    BG_WINDOW_TILES,
    WINDOW_ENABLE,
    WINDOW_TILE_MAP,
    LCD_PPU_ENABLE
} LCDC_BITS;

typedef struct {
    uint8_t lcdc;
    uint8_t lcds;
    uint8_t ly;
    uint8_t lyc;
    uint8_t dma_enable;
} LCD;

void lcdWrite(uint16_t addr, uint8_t val);
uint8_t lcdRead(uint16_t addr);
