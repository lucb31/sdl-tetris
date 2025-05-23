//
// Created by lucas on 23.05.25.
//

#include "Board.h"

namespace Tetris {
    void Board::CalculateCollisions() {
        m_collisions.clear();
        for (const std::shared_ptr<Shape> &a: m_shapes) {
            for (const std::shared_ptr<Shape> &b: m_shapes) {
                // Skip collision with self
                if (&a == &b) {
                    continue;
                }
                // TODO: Skip if combination already checked
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

    bool Board::AddShape(Shape *shape) {
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
