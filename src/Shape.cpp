//
// Created by lucas on 23.05.25.
//

#include "Shape.h"

namespace Tetris {
    Shape::Shape(float x, float y) {
        m_velocity = Math::Vec2(0, 20);
        m_position = Math::Vec2(x, y);
        m_grounded = false;
    }

    void Shape::Draw(SDL_Surface *surf) {
        const SDL_Rect rect = {(int) m_position.x(), (int) m_position.y(), 50, 50};
        SDL_FillSurfaceRect(surf, &rect, SDL_MapSurfaceRGB(surf, 0x00, 0xFF, 0xFF));
    }

    void Shape::Tick(const float dt) {
        m_position += m_velocity * dt;

        // Collision with floor
        if (m_position.y() >= 500) {
            m_grounded = true;
            m_velocity = Math::Vec2();
        }
    }
} // Tetris
