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
        // 4 tiles per shape + 4 boundary BB
        bbs.reserve(m_shapes.size() * 4 + 4);
        for (const auto &shape: m_shapes) {
            const auto shapeBBs = shape->GetCollisionBBs();
            for (const auto &bb: shapeBBs) {
                bbs.emplace_back(std::make_shared<ShapeWithBB>(shape, bb));
            }
        }

        // Add board bounding boxes
        const auto groundBB = std::make_shared<SDL_FRect>(0.0f, boardSizeY, boardSizeX, 10.0f);
        bbs.emplace_back(std::make_shared<ShapeWithBB>(nullptr, groundBB));
        const auto topBB = std::make_shared<SDL_FRect>(0.0f, -10.0f, boardSizeX, 9.9f);
        bbs.emplace_back(std::make_shared<ShapeWithBB>(nullptr, topBB));
        const auto leftBB = std::make_shared<SDL_FRect>(-10.0f, 0.0f, 10.0f, boardSizeY);
        bbs.emplace_back(std::make_shared<ShapeWithBB>(nullptr, leftBB));
        const auto rightBB = std::make_shared<SDL_FRect>(boardSizeX, 0.0f, 10.0f, boardSizeY);
        bbs.emplace_back(std::make_shared<ShapeWithBB>(nullptr, rightBB));

        // Iterate over unique combinations of bounding boxes
        for (int i = 0; i < bbs.size(); i++) {
            for (int j = i + 1; j < bbs.size(); j++) {
                auto a = bbs[i];
                auto b = bbs[j];
                // No need to check collisions between shapes that are grounded or outer bounds
                if ((a->shape == nullptr || a->shape->IsGrounded()) &&
                    (b->shape == nullptr || b->shape->IsGrounded()))
                    continue;
                // Skip collision check for tiles within same shape
                if (a->shape == b->shape) continue;
                SDL_FRect intersection{};
                if (SDL_GetRectIntersectionFloat(a->bb.get(), b->bb.get(), &intersection)) {
                    // Collision detected
                    // Ensure that the first shape of the collision is always the active one
                    if (a->shape == nullptr || a->shape->IsGrounded()) {
                        a.swap(b);
                    }

                    // Determine on which side of the BB we've collided
                    const std::shared_ptr<SDL_FRect> bbForDirection = a->bb;
                    // Check position of intersection relative to position
                    Math::Vec2 collisionDirection;
                    if (intersection.w >= 2.0f) {
                        // Vertical collision
                        if (bbForDirection->y >= intersection.y) {
                            // Collision on the top side of the shape
                            collisionDirection.e[1] = -1;
                        } else {
                            // Collision on the bottom side of the shape
                            collisionDirection.e[1] = 1;
                        }
                    }
                    if (intersection.h >= 2.0f) {
                        // Horizontal collision
                        if (bbForDirection->x >= intersection.x) {
                            // Collision on the left side
                            collisionDirection.e[0] = -1;
                        } else {
                            // Collision on the right side
                            collisionDirection.e[0] = 1;
                        }
                    }

                    // Register collision to be handled in next process step
                    m_collisions.emplace_back(Collision{a->shape, b->shape, collisionDirection});
                }
            }
        }
    }

    void Board::ProcessCollisions() {
        for (const auto &collision: m_collisions) {
            if (collision.direction.y() != 0) {
                // Vertical collision
                // Stop all movement of involved shapes
                if (collision.a != nullptr) collision.a->Freeze();
                if (collision.b != nullptr) collision.b->Freeze();

                // Collision on top side of the shape -> Game over
                if (collision.direction.y() < 0) {
                    m_gameOver = true;
                }
            }
            if (collision.direction.x() != 0) {
                // Horizontal collision -> Restrict input movement
                m_leftPressed = m_leftPressed && collision.direction.x() > 0;
                m_rightPressed = m_rightPressed && collision.direction.x() < 0;
            }
        }
    }

    Board::Board() {
        AddRandomShape();
    }

    void Board::AddRandomShape() {
        m_activeShape = std::make_shared<Shape>(tileCols / 2 * widthPerTile, 0);
        m_shapes.emplace_back(m_activeShape);
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
        if (m_gameOver) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDebugText(renderer, boardSizeX / 2, boardSizeY / 2, "Game Over!");
            return;
        }
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
            if (m_activeShape->IsGrounded() && !m_gameOver) {
                // Set timer to spawn next shape
                m_tickTimer = std::make_unique<TickTimer>(1.0f, [this](int) {
                    AddRandomShape();
                });
                m_activeShape = nullptr;
            } else {
                // Apply active shape movement inputs
                auto inputVelocity = Math::Vec2();
                if (m_leftPressed)
                    inputVelocity += Math::Vec2(-1, 0);
                if (m_rightPressed)
                    inputVelocity += Math::Vec2(1, 0);
                if (m_downPressed)
                    inputVelocity += Math::Vec2(0, 1);
                m_activeShape->AddInputVelocity(inputVelocity);
            }
        }

        // Calculate frame for all subobjects
        if (m_tickTimer != nullptr) {
            m_tickTimer->Tick(dt);
        }
        // Shapes
        for (const std::shared_ptr<Shape> &shape: m_shapes) {
            shape->Tick(dt);
        }
    }
} // Tetris
