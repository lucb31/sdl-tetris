//
// Created by lucas on 23.05.25.
//

#ifndef SHAPE_H
#define SHAPE_H
#include "GameObject.h"
#include "Vec2.h"
#include "Constants.h"

namespace Tetris {
    class Shape : public GameObject {
        // Width & height in px
        float m_width, m_height;
        // Constant vertical velocity component applied to velocity calculation
        float m_gravity;
        // Controls how 'maneuverable' the shape is
        float m_speed;
        bool m_grounded;

        Math::Vec2 m_position;
        // Actual velocity
        Math::Vec2 m_velocity;
        // Requested velocity by user input
        Math::Vec2 m_inputVelocity;

        void CalculateVelocity();

    public:
        SDL_FRect DiscreteBB() const;

        Shape() : Shape(0, 0) { }

        Shape(const float x, const float y) : m_velocity(Math::Vec2(0, 0)),
                                              m_position(Math::Vec2(x, y)),
                                              m_width(widthPerTile),
                                              m_height(heightPerTile),
                                              m_grounded(false),
                                              m_gravity(150),
                                              m_speed(150) { }

        bool IsGrounded() const { return m_grounded; }

        void Freeze();

        void AddInputVelocity(const Math::Vec2 &direction);

        void Draw(SDL_Renderer *) override;

        void Tick(float dt) override;
    };
} // Tetris

#endif //SHAPE_H
