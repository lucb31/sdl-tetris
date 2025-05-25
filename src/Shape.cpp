//
// Created by lucas on 23.05.25.
//

#include "Shape.h"

namespace Tetris {
    void Shape::CalculateVelocity() {
        m_velocity = m_inputVelocity + Math::Vec2(0, m_gravity);
    }

    SDL_Rect Shape::BB() const {
        return SDL_Rect{(int) m_position.x(), (int) m_position.y(), (int) m_width, (int) m_height};
    }

    SDL_FRect Shape::FBB() const {
        return SDL_FRect{m_position.x(), m_position.y(), m_width, m_height};
    }

    void Shape::Freeze() {
        m_velocity = Math::Vec2();
        m_grounded = true;
    }

    void Shape::MoveTowards(const Math::Vec2 &direction) {
        if (m_grounded) {
            return;
        }
        Math::Vec2 normalizedDirection = direction.Norm();
        m_inputVelocity = normalizedDirection*m_speed;
    }

    void Shape::Draw(SDL_Renderer* renderer) {
        // Simply draw by drawing filled BB
        SDL_FRect bb = FBB();
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
        if (m_position.y() >= 500) {
            Freeze();
        }
    }
} // Tetris
