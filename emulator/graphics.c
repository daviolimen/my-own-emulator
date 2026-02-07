//
// Created by davi on 2/7/26.
//

#include "graphics.h"

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 128

SDL_Window* window;
SDL_Texture* texture;
SDL_Renderer* renderer;

uint32_t grayPalette[256];
uint32_t sdlFramebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

int initWindow() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow(
        "My Own Emulator!",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
        );

    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );

    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
        );

    for (int i = 0; i < 256; i++) {
        grayPalette[i] = 0xFF000000 | (i << 16) | (i << 8) | i;
    }

    return 0;
}

int windowLoop(const uint8_t* framebuffer) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        sdlFramebuffer[i] = grayPalette[framebuffer[i]];
    }

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return -1;
        }
    }

    void* pixels;
    int pitch;
    SDL_LockTexture(texture, NULL, &pixels, &pitch);
    memcpy(pixels, sdlFramebuffer, SCREEN_HEIGHT * pitch);
    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    return 0;
}