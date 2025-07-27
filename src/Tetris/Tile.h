//
// Created by lucas on 25.05.25.
//

#ifndef TILE_H
#define TILE_H
#include "../GameObject.h"
#include "glm/glm.hpp"
#include "SDL3/SDL_rect.h"

namespace Tetris {
    class Tile : public GameObject {
        glm::vec3 GetGlobalTopLeft() const;

    public:
        // Relative position towards parent
        glm::vec2 position{0.0f};
        glm::mat4 parentTransform{1.0f};

        SDL_FRect BB() const;

        Tile() : Tile(0.0f, 0.0f) {
        };

        Tile(float x, float y) : Tile(glm::vec2(x, y)) {
        };

        explicit Tile(const glm::vec2 &pos) : position(pos), parentTransform(glm::mat4(1.0f)) {
        };

        // Nothing to do in draw function. Rendered in one draw call within shape
        void Draw() override {
        };

        // Nothing to do in tick function. Movement & behavior is handled in parent
        void Tick(float dt) override {
        }
    };
} // Tetris

#endif //TILE_H
