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

    void Shape::Draw(SDL_Surface *surf) {
        // Simply draw by drawing filled BB
        const SDL_Rect bb = BB();
        SDL_FillSurfaceRect(surf, &bb, SDL_MapSurfaceRGB(surf, 0x00, 0xFF, 0xFF));
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
