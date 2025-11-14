#include "stdio.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

SDL_Window * game_window;
SDL_Renderer * renderer;
SDL_Surface * surface;
SDL_Window * debug_window;

static void guiInitDebugWindow(void) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Debug Window Init Successful\n");
    }
    if(TTF_Init()) {
        printf("TTF Init Successful\n");
    }
}

static bool guiInitWindow(void) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL Main Window Init Successful\n");
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

bool guiInit(void) {
    return guiInitWindow();
}