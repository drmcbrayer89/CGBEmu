#include "common.h"

#define VRAM_START 0x8000
#define OAM_START  0xFE00

typedef enum {
    HBLANK,
    VBLANK,
    DRAWING,
    OAM_SEARCH
} PPU_MODES;

typedef struct {
    uint8_t y;
    uint8_t x;
    uint8_t tile_index;

    union {
        uint8_t word;

        struct {
            uint8_t cgb_palette : 3;
            uint8_t bank : 1;
            uint8_t dmg_palette : 1;
            uint8_t x_flip : 1;
            uint8_t y_flip : 1;
            uint8_t priority : 1;
        } bits;
    } attributes;
} PPU_OAM;

typedef struct {
    uint8_t vram[0x1FFF];
    PPU_OAM oam[0x9F];
    uint32_t ticks;
    bool oam_locked;
    bool vram_locked;
    PPU_MODES mode;
    uint8_t ly;
    uint8_t x;
} PPU;

void ppuTick(void);
void ppuGetColorIndexes(uint16_t line, uint8_t * color_id_out);
void ppuWriteOam(uint16_t addr, uint8_t val);
uint8_t ppuReadOam(uint16_t addr);
void ppuWriteVram(uint16_t addr, uint8_t val);
uint8_t ppuReadVram(uint16_t addr);
void ppuInit(void);