//
// Created by lucas on 23.05.25.
//

#include "Board.h"

#include "Keymap.h"
#include "Constants.h"

namespace Tetris {
    struct ShapeWithBB {
        std::shared_ptr<Shape> shape;
        std::shared_ptr<SDL_FRect> bb;
    };

    void Board::CalculateCollisions() {
        m_collisions.clear();
        // Retrieve list of bounding boxes
        std::vector<std::shared_ptr<ShapeWithBB> > bbs;
        bbs.reserve(m_shapes.size() * 4);
        for (const auto &shape: m_shapes) {
            const auto shapeBBs = shape->GetCollisionBBs();
            for (const auto &bb: shapeBBs) {
                bbs.emplace_back(std::make_shared<ShapeWithBB>(shape, bb));
            }
        }

        // Iterate over unique combinations of bounding boxes
        for (int i = 0; i < bbs.size(); i++) {
            for (int j = i + 1; j < bbs.size(); j++) {
                const auto &a = bbs[i];
                const auto &b = bbs[j];
                // No need to check collisions between two grounded shapes
                if (a->shape->IsGrounded() && b->shape->IsGrounded()) continue;
                // Skip collision check for tiles within same shape
                if (a->shape == b->shape) continue;
                SDL_FRect intersection{};
                if (SDL_GetRectIntersectionFloat(a->bb.get(), b->bb.get(), &intersection)) {
                    // Collision detected
                    m_collisions.emplace_back(Collision{a->shape, b->shape, intersection});
                }
            }
        }
    }

    void Board::ProcessCollisions() const {
        for (const auto &collision: m_collisions) {
            const bool isVerticalCollision = collision.intersection.w > 0;
            if (isVerticalCollision) {
                // Handle collisions by stopping all movement of involved shapes
                collision.a->Freeze();
                collision.b->Freeze();
            }
        }
    }

    Board::Board() {
        AddRandomShape();
    }

    void Board::AddRandomShape() {
        m_activeShape = std::make_shared<Shape>(tileCols / 2 * widthPerTile, 0);
        AddShape(m_activeShape);
    }

    bool Board::AddShape(const std::shared_ptr<Shape> &shape) {
        m_shapes.emplace_back(shape);
        return true;
    }

    void Board::DrawGrid(SDL_Renderer *renderer) {
        // NOTE: Optimization: We could just calculate and store the grid data. This never changes
        std::vector<SDL_FRect> rects;
        rects.reserve(tiles);
        for (int row = 0; row < tileRows; row++) {
            for (int col = 0; col < tileCols; col++) {
                rects.emplace_back(col * widthPerTile, row * heightPerTile, widthPerTile, heightPerTile);
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 25);
        SDL_RenderRects(renderer, rects.data(), tiles);
    }

    void Board::HandleKeyDown(const SDL_KeyboardEvent &e) {
        if (e.key == MoveLeft) {
            m_leftPressed = true;
        } else if (e.key == MoveRight) {
            m_rightPressed = true;
        } else if (e.key == MoveDown) {
            m_downPressed = true;
        } else if (e.key == RotateLeft) {
            if (m_activeShape != nullptr) {
                m_activeShape->Rotate(M_PI / 2);
            }
        } else if (e.key == RotateRight) {
            if (m_activeShape != nullptr) {
                m_activeShape->Rotate(-M_PI / 2);
            }
        }
    }

    void Board::HandleKeyUp(const SDL_KeyboardEvent &e) {
        if (e.key == MoveLeft) {
            m_leftPressed = false;
        } else if (e.key == MoveRight) {
            m_rightPressed = false;
        } else if (e.key == MoveDown) {
            m_downPressed = false;
        }
    }

    void Board::Draw(SDL_Renderer *renderer) {
        DrawGrid(renderer);
        // Draw shapes
        for (const std::shared_ptr<Shape> &shape: m_shapes) {
            shape->Draw(renderer);
        }
    }

    void Board::Tick(const float dt) {
        CalculateCollisions();
        ProcessCollisions();

        if (m_activeShape != nullptr) {
            // Active shape movement inputs
            auto inputVelocity = Math::Vec2();
            if (m_leftPressed)
                inputVelocity += Math::Vec2(-1, 0);
            if (m_rightPressed)
                inputVelocity += Math::Vec2(1, 0);
            if (m_downPressed)
                inputVelocity += Math::Vec2(0, 1);
            m_activeShape->AddInputVelocity(inputVelocity);

            // Set timer to spawn new shape once active shape is grounded
            if (m_activeShape->IsGrounded()) {
                m_tickTimer = std::make_unique<TickTimer>(1.0f, [this](int) {
                    AddRandomShape();
                });
                m_activeShape = nullptr;
            }
        }

        // Calculate frame for all subobjects
        if (m_tickTimer != nullptr) {
            m_tickTimer->Tick(dt);
        }
        for (const std::shared_ptr<Shape> &shape: m_shapes) {
            shape->Tick(dt);
        }
    }
} // Tetris
