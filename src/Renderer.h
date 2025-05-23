//
// Created by lucas on 12.05.25.
//

#ifndef RENDERER_H
#define RENDERER_H
#endif //RENDERER_H

#include "Board.h"
#include <SDL3/SDL.h>

constexpr int kScreenWidth = 1024;
constexpr int kScreenHeight = 768;


class Renderer {
public:
    bool init();
    bool render(GameObject *);
    void close();

private:
    SDL_Window* window{nullptr};
    SDL_Surface* screen_surface{nullptr};
    SDL_Surface* hello_world{nullptr};
};