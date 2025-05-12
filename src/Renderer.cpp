//
// Created by lucas on 12.05.25.
//

#include "Renderer.h"

bool Renderer::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize. Received error %s\n", SDL_GetError());
        return false;
    }
    if (window = SDL_CreateWindow("SDL3 Hello world", kScreenWidth, kScreenHeight, 0); window == nullptr) {
        SDL_Log("SDL could not initialize window. Received error %s\n", SDL_GetError());
        return false;
    }

    screen_surface = SDL_GetWindowSurface(window);
    return true;
}

void Renderer::close() {
    SDL_DestroySurface(hello_world);
    hello_world = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;
    screen_surface = nullptr;

    SDL_Quit();
}

bool Renderer::render() {
    SDL_FillSurfaceRect(screen_surface, nullptr, SDL_MapSurfaceRGB(screen_surface, 0xFF, 0xFF, 0xFF));
    const SDL_Rect rect = {100, 100, 250, 250};
    SDL_FillSurfaceRect(screen_surface, &rect, SDL_MapSurfaceRGB(screen_surface, 0x00, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);
    return true;
}


