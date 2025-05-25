//
// Created by lucas on 25.05.25.
//

#ifndef TILE_H
#define TILE_H
#include "GameObject.h"
#include "Mat3.h"
#include "Vec2.h"
#include "SDL3/SDL_rect.h"

namespace Tetris {

class Tile : public GameObject {
    // Relative position towards parent
    Math::Vec2 m_position;
public:
    Math::Mat3 parentTransform{1,0,0, 0,1,0, 0,0,1};

    SDL_FRect BB() const;

    Tile() : Tile(0, 0) {};
    Tile(float x, float y) : m_position(x, y) {};
    void Draw(SDL_Renderer *) override;

    // Nothing to do in tick function. Movement & behavior is handled in parent
    void Tick(float dt) override {}
};

} // Tetris

#endif //TILE_H
