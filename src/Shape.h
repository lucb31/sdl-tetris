//
// Created by lucas on 23.05.25.
//

#ifndef SHAPE_H
#define SHAPE_H
#include <memory>
#include <vector>

#include "GameObject.h"
#include "Tile.h"

namespace Tetris {
    constexpr glm::vec2 GRAVITY_VECTOR = glm::vec2(0.0f, 150.0f);

    class Shape : public GameObject {
        // Controls how 'maneuverable' the shape is
        float m_speed{150};
        bool m_grounded{false};

        // Tiles need to be shared ptrs since they're going to get
        // passed to the board once shape is placed
        std::vector<std::shared_ptr<Tile> > m_tiles;
        // Current position
        glm::vec2 m_position{0.0f};
        // Current rotation in RAD
        float m_rotation{0.0f};
        // Current velocity
        glm::vec2 m_velocity{0.0f};
        // Requested velocity by user input
        glm::vec2 m_inputVelocity{0.0f};

        void CalculateVelocity();

    public:
        Shape(float x, float y, const std::vector<glm::vec2> &tilePositions);

        bool IsGrounded() const { return m_grounded; }

        std::vector<std::shared_ptr<Tile> > GetTiles() const { return m_tiles; }

        void Freeze();

        void AddInputVelocity(const glm::vec2 &direction);

        void Rotate(float radians);

        void Draw() override;

        void Tick(float dt) override;

        std::vector<SDL_FRect> GetCollisionBBs() const;

        glm::mat4 GetTransform() const;
    };
} // Tetris

#endif //SHAPE_H
