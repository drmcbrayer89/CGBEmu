#include "stdio.h"
#include "gb.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

SDL_Window * p_game_window;
SDL_Renderer * p_renderer;
SDL_Surface * p_surface;
SDL_Window * p_debug_window;

GB * p_gb;

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
        p_game_window = SDL_CreateWindow("CGBEmu", 640, 480, SDL_WINDOW_OPENGL);
        p_renderer = SDL_CreateRenderer(p_game_window, NULL);
        
        SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);

        if(p_game_window == NULL) {
            printf("Error creating window\n");
            return false;
        }
        return true;
    }
    return false;
}

void guiRun() {
    SDL_Event e;

    while(SDL_PollEvent(&e) > 0){
        SDL_RenderPresent(p_renderer);
        if(e.type == SDL_EVENT_QUIT) {
            p_gb->stop_emu = true;
        }
    }
}

bool guiInit(void) {
    gbGetGbPtr(p_gb);
    return guiInitWindow();
}