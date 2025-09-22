#include "gb.h"
#include "cpu.h"

GB gameboy;

void gbTick(void) {
    // implement later with tick accuracy
    uint16_t i = 0;
    for(i; i < CYCLES_PER_BYTE; i++) {
        gameboy.ticks++;
        // sync with PPU later :)
    }
}

void gbStart(void) {
    /* start cpu */
    cpuInit();
    while(gameboy.running)
    {
        if(cpuStep() == false) {
            gameboy.running = false;
        }
    }
    printf("CGBEmu Stopped!\n");
}

void gbInit(void) {
    /* Setup SDL & shit later */
    gameboy.running = true;
    gameboy.halted = false;
}