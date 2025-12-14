#include "common.h"

#define VRAM_START 0x8000
#define OAM_START  0xFE00

typedef enum {
    MODE_0,
    MODE_1,
    MODE_2,
    MODE_3
} PPU_MODES;

typedef struct {
    uint8_t vram[0x1FFF];
    uint8_t oam[0x9F];
    uint32_t dots;
    bool oam_locked;
    bool vram_locked;
    PPU_MODES mode;
} PPU;

void ppuTick(void);
void ppuGetColorIndexes(uint16_t line, uint8_t * color_id_out);
void ppuWriteOam(uint16_t addr, uint8_t val);
uint8_t ppuReadOam(uint16_t addr);
void ppuWriteVram(uint16_t addr, uint8_t val);
uint8_t ppuReadVram(uint16_t addr);
void ppuInit(void);