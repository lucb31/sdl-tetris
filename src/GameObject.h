//
// Created by lucas on 23.05.25.
//

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "SDL3/SDL_surface.h"

class GameObject {
public:
    virtual void Draw(SDL_Surface*) = 0;
    virtual void Tick(float dt) = 0;
};
#endif //GAMEOBJECT_H
