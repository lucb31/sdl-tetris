//
// Created by lucas on 23.05.25.
//

#ifndef SHAPE_H
#define SHAPE_H
#include <memory>
#include <vector>

#include "GameObject.h"
#include "Vec2.h"
#include "Constants.h"
#include "Mat3.h"
#include "Tile.h"

namespace Tetris {
    class Shape : public GameObject {
        // Constant vertical velocity component applied to velocity calculation
        float m_gravity;
        // Controls how 'maneuverable' the shape is
        float m_speed;
        bool m_grounded = false;

        std::vector<std::unique_ptr<Tile> > m_tiles;
        Math::Vec2 m_position;
        float m_rotation = 0.0f;
        // Actual velocity
        Math::Vec2 m_velocity;
        // Requested velocity by user input
        Math::Vec2 m_inputVelocity;

        void CalculateVelocity();

    public:
        Shape() : Shape(0, 0) { }

        Shape(const float x, const float y) : m_velocity(Math::Vec2(0, 0)),
                                              m_position(Math::Vec2(x, y)),
                                              m_gravity(150),
                                              m_speed(150) {
            // Initialize tiles
            m_tiles.reserve(4);
            for (int i = 0; i < 4; i++) {
                m_tiles.emplace_back(std::make_unique<Tile>(0, i * heightPerTile));
            }
        }

        bool IsGrounded() const { return m_grounded; }

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
