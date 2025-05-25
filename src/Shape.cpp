//
// Created by lucas on 23.05.25.
//

#include "Shape.h"

#include <cmath>

namespace Tetris {
    void Shape::CalculateVelocity() {
        m_velocity = m_inputVelocity + Math::Vec2(0, m_gravity);
    }

    SDL_FRect Shape::DiscreteBB() const {
        const float discreteY = std::round(m_position.y() / heightPerTile) * heightPerTile;
        const float discreteX = std::round(m_position.x() / widthPerTile) * widthPerTile;
        return SDL_FRect{discreteX, discreteY, m_width, m_height};
    }

    void Shape::Freeze() {
        m_velocity = Math::Vec2();
        m_grounded = true;
    }

    void Shape::AddInputVelocity(const Math::Vec2 &direction) {
        if (m_grounded) {
            return;
        }
        Math::Vec2 normalizedDirection = direction.Norm();
        m_inputVelocity = normalizedDirection*m_speed;
    }

    void Shape::Draw(SDL_Renderer* renderer) {
        // Simply draw by drawing filled BB
        SDL_FRect bb = DiscreteBB();
        // TODO: Enums for colors
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderFillRect(renderer, &bb);
    }

    void Shape::Tick(const float dt) {
        if (m_grounded) {
            return;
        }
        CalculateVelocity();
        m_position += m_velocity * dt;

        // Collision with floor
        // TODO: Ground plane
        if (m_position.y() >= 500) {
            Freeze();
        }
    }
} // Tetris
