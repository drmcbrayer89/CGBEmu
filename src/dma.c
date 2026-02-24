#include "dma.h"
#include "ppu.h"
#include "bus.h"

static DMA dma;

void dmaWrite(uint8_t val) {
    dma.active = true;
    dma.val = val;
    dma.delay = 2;
    dma.byte = 0;
}

void dmaTick(void) {
    if(dma.active) {
        uint8_t data = busReadDmaAddr((dma.val * 0x100) + dma.byte);
        ppuWriteOam(dma.byte, data);

        dma.byte++;

        if(dma.byte >= 0xA0) {
            dma.active = false;
        }
    }
}

bool dmaIsActive(void) {
    return dma.active;
}