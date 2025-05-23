//
// Created by lucas on 23.05.25.
//

#include "Board.h"

namespace Tetris {
    bool Board::AddShape(Shape *shape) {
        m_shapes.emplace_back(shape);
        return true;
    }

    void Board::Draw(SDL_Surface *surf) {
        for (const std::shared_ptr<Shape>&shape : m_shapes) {
            shape->Draw(surf);
        }
    }

    void Board::Tick(const float dt) {
        for (const std::shared_ptr<Shape>&shape : m_shapes) {
            shape->Tick(dt);
        }
    }
} // Tetris