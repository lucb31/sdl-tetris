//
// Created by lucas on 23.05.25.
//

#include "Board.h"

namespace Tetris {
    void Board::CalculateCollisions() {
        m_collisions.clear();
        // Iterate over unique combinations of shapes
        for (int i = 0; i < m_shapes.size(); i++) {
            for (int j = i + 1; j < m_shapes.size(); j++) {
                const std::shared_ptr<Shape> a = m_shapes[i];
                const std::shared_ptr<Shape> b = m_shapes[j];
                SDL_Rect a_bb = a->BB();
                SDL_Rect b_bb = b->BB();
                SDL_Rect intersection{};
                if (SDL_GetRectIntersection(&a_bb, &b_bb, &intersection)) {
                    // Collision detected
                    m_collisions.emplace_back(Collision{a, b, intersection});
                }
            }
        }
    }

    void Board::ProcessCollisions() {
        for (const auto &collision: m_collisions) {
            // Handle collisions by stopping all movement of involved shapes
            collision.a->Freeze();
            collision.b->Freeze();
        }
    }

    bool Board::AddShape(const std::shared_ptr<Shape>& shape) {
        m_shapes.emplace_back(shape);
        return true;
    }

    void Board::Draw(SDL_Surface *surf) {
        for (const std::shared_ptr<Shape> &shape: m_shapes) {
            shape->Draw(surf);
        }
    }

    void Board::Tick(const float dt) {
        CalculateCollisions();
        ProcessCollisions();
        for (const std::shared_ptr<Shape> &shape: m_shapes) {
            shape->Tick(dt);
        }
    }
} // Tetris
