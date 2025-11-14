#include "gb.h"
#include "cpu.h"
#include "gui.h"
#include "pthread.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

// Start doing SDL shit here
GB gameboy;

pthread_t t_cpu;

static void delay(uint32_t t) {
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

void * thread_cpu(void *) {
    cpuInit();
    while(gameboy.running){
        gameboy.running = cpuStep();
        gameboy.ticks++;
    }
}

void gbStart(void) {
    /* start gui */
    guiInit();
    /* start cpu */
    pthread_create(&t_cpu, NULL, thread_cpu, NULL);
    printf("CGBEmu Stopped!\n");
}

void gbInit(void) {
    /* Setup SDL & shit later */
    gameboy.running = true;
    gameboy.halted = false;
}