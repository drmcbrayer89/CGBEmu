#include "gb.h"
#include "cpu.h"
#include "gui.h"
#include "pthread.h"
#include "unistd.h"

GB gameboy;
pthread_t t_cpu;

void gbGetGbPtr(GB * p_gb) {
    p_gb = &gameboy;
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
    if(guiInit()) {
        printf("GUI Initialized\n");
    }
    
    /* start cpu */
    if(pthread_create(&t_cpu, NULL, thread_cpu, NULL)) {
        printf("CPU thread failed to start\n");
        exit(-1);
    }

    while(!gameboy.stop_emu) {
        gameboy.stop_emu = guiRun();
    }

    printf("CGBEmu Stopped!\n");
}

void gbInit(void) {
    /* Setup SDL & shit later */
    gameboy.running = true;
    gameboy.halted = false;
    gameboy.stop_emu = false;
}