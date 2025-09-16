#include "gb.h"
#include "cpu.h"

uint64_t ticks = 0;

void gbTick(void) {
    // implement later with tick accuracy
    uint16_t i = 0;
    for(i; i < CYCLES_PER_BYTE; i++) {
        ticks++;
        // sync with PPU later :)
    }
}