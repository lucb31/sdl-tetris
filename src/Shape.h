//
// Created by lucas on 23.05.25.
//

#ifndef SHAPE_H
#define SHAPE_H
#include <memory>
#include <vector>

#include "GameObject.h"
#include "Vec2.h"
#include "Mat3.h"
#include "Tile.h"

namespace Tetris {
    class Shape : public GameObject {
        // Constant vertical velocity component applied to velocity calculation
        float m_gravity = 150;
        // Controls how 'maneuverable' the shape is
        float m_speed = 150;
        bool m_grounded = false;

        std::vector<std::shared_ptr<Tile> > m_tiles;
        Math::Vec2 m_position;
        float m_rotation = 0.0f;
        // Actual velocity
        Math::Vec2 m_velocity;
        // Requested velocity by user input
        Math::Vec2 m_inputVelocity;

        void CalculateVelocity();

    public:
        Shape(float x, float y, const std::vector<Math::Vec2> &tilePositions);

        bool IsGrounded() const { return m_grounded; }

        std::vector<std::shared_ptr<Tile>> GetTiles() const { return m_tiles; }

        void Freeze();

        void AddInputVelocity(const Math::Vec2 &direction);

        void Rotate(float radians);

        void Draw(SDL_Renderer *) override;

        void Tick(float dt) override;

        std::vector<std::shared_ptr<SDL_FRect>> GetCollisionBBs() const;

        Math::Mat3 GetTransform() const;

    };
} // Tetris

#endif //SHAPE_H
