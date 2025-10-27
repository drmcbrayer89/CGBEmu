#include "gb.h"
#include "cpu.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

// Start doing SDL shit here
GB gameboy;

void delay(uint32_t t) {
    SDL_Delay(t);
}
void gbTick(uint8_t cycles) {
    // implement later with tick accuracy
    uint16_t i = 0;
    uint16_t j = 0;
    for(i; i < cycles; i++){
        for(j; j < CYCLES_PER_BYTE; j++) {
            gameboy.ticks++;
            // sync with PPU later :)
        }
    }
}

void gbStart(void) {
    /* create window */
    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Init Successful\n");
    }
    if(TTF_Init()) {
        printf("TTF Init Successful\n");
    }
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