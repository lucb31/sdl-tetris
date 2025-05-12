//
// Created by lucas on 12.05.25.
//

#ifndef RENDERER_H
#define RENDERER_H

#endif //RENDERER_H

#include <SDL3/SDL.h>

constexpr int kScreenWidth = 800;
constexpr int kScreenHeight = 600;

class Renderer {
public:
    bool init();
    bool render();
    void close();

private:
    SDL_Window* window{nullptr};
    SDL_Surface* screen_surface{nullptr};
    SDL_Surface* hello_world{nullptr};
};