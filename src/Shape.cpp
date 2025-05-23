//
// Created by lucas on 23.05.25.
//

#include "Shape.h"

namespace Tetris {
    SDL_Rect Shape::BB() const {
        return SDL_Rect{(int) m_position.x(), (int) m_position.y(), (int) m_width, (int) m_height};
    }

    Shape::Shape(float x, float y) {
        m_velocity = Math::Vec2(0, 50);
        m_position = Math::Vec2(x, y);
        m_width = 50;
        m_height = 50;
        m_grounded = false;
    }

    void Shape::Freeze() {
        m_velocity = Math::Vec2();
        m_grounded = true;
    }

    void Shape::Draw(SDL_Surface *surf) {
        // Simply draw by drawing filled BB
        const SDL_Rect bb = BB();
        SDL_FillSurfaceRect(surf, &bb, SDL_MapSurfaceRGB(surf, 0x00, 0xFF, 0xFF));
    }

    void Shape::Tick(const float dt) {
        m_position += m_velocity * dt;

        // Collision with floor
        if (m_position.y() >= 500) {
            Freeze();
        }
    }
} // Tetris
