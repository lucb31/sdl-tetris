//
// Created by lucas on 27.07.25.
//

#ifndef COLLISION_H
#define COLLISION_H
#include "SDL3/SDL_rect.h"

struct Collision {
    SDL_FRect a;
    SDL_FRect b;
    SDL_FRect intersection;
};

#endif //COLLISION_H
