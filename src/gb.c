#include "gb.h"
#include "cpu.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

// Start doing SDL shit here
GB gameboy;
SDL_Window * game_window;
SDL_Renderer * renderer;
SDL_Window * debug_window;

static void delay(uint32_t t) {
    SDL_Delay(t);
}

static void gbInitDebugWindow(void) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Init Successful\n");
    }
    if(TTF_Init()) {
        printf("TTF Init Successful\n");
    }
}

static bool gbInitWindow(void) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Init Successful\n");
    }
    game_window = SDL_CreateWindow("CGBEmu", 320, 160, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(game_window, NULL);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPresent(renderer);
    
    if(game_window == NULL) {
        printf("Error creating window\n");
        return false;
    }
    return true;
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
    if(gbInitWindow() == false) {
        printf("SDL Game Window Init FAILED\n");
        exit(-1);
    }
    gbInitDebugWindow();
    /* start cpu */
    cpuInit();
    while(gameboy.running)
    {
        delay(100);
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