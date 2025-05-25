//
// Created by lucas on 12.05.25.
//

#include "Renderer.h"

bool Renderer::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize. Received error %s\n", SDL_GetError());
        return false;
    }
    if (!SDL_CreateWindowAndRenderer("Tetris", kScreenWidth, kScreenHeight, 0, &m_window, &m_renderer)) {
        SDL_Log("SDL could not initialize window or renderer. Received error %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void Renderer::close() {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    SDL_Quit();
}

bool Renderer::render(GameObject *d) {
    // Clear screen
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    // Draw world
    d->Draw(m_renderer);
    SDL_RenderPresent(m_renderer);
    return true;
}


