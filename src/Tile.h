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

    Math::Vec3 GetGlobalTopLeft() const;

    void DrawRealPosition(SDL_Renderer *renderer) const;

    void DrawDiscretePosition(SDL_Renderer *renderer) const;
public:
    // Relative position towards parent
    Math::Vec2 position;
    Math::Mat3 parentTransform{1,0,0, 0,1,0, 0,0,1};

    SDL_FRect BB() const;

    Tile() : Tile(0, 0) {};
    Tile(float x, float y) : position(x, y) {};
    explicit Tile(const Math::Vec2 &pos) : position(pos) {};

    void Draw() override;

    // Nothing to do in tick function. Movement & behavior is handled in parent
    void Tick(float dt) override {}
};

} // Tetris

#endif //TILE_H
