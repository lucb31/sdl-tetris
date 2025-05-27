//
// Created by lucas on 23.05.25.
//

#include "Board.h"

#include "Keymap.h"
#include "Constants.h"
#include "SDL3/SDL_log.h"

namespace Tetris {
    void Board::ClearLines(const std::vector<int> &rowIndices) {
        for (const auto &rowIndex: rowIndices) {
            // Separate list of survivors that will replace the
            // list of tiles after every row iteration
            std::vector<std::shared_ptr<Tile>> survivors;
            survivors.reserve(m_tiles.size() - tileCols);

            // Bounds for the current row
            const int rowMinY = rowIndex * heightPerTile;
            const int rowMaxY = (rowIndex + 1) * heightPerTile;
            for (const auto &tile : m_tiles) {
                if (tile->position.y() > rowMaxY) {
                    // If below row -> Survives
                    survivors.push_back(tile);
                    SDL_Log("Tile below cleared row");
                } else if (tile->position.y() < rowMinY) {
                    // If above row -> Survive & Move
                    tile->position += Math::Vec2(0, heightPerTile);
                    survivors.push_back(tile);
                    SDL_Log("Tile above cleared row");
                } else {
                    SDL_Log("Tile within cleared row");
                    // Will be removed, by not adding it to the survivors
                }
            }
            // Update tiles with survivor list
            m_tiles = survivors;
        }
    }

    void Board::CheckForClearedLines() {
        // Retrieve list of all frozen tile bounding boxes
        std::vector<SDL_FRect> tileBBs;
        tileBBs.reserve(m_tiles.size());
        for (const auto &tile: m_tiles) {
            tileBBs.push_back(tile->BB());
        }

        // Iterate over every scanline and check for collisions with tile BBs
        std::vector<int> rowsToClear;
        for (int row = 0; row < tileRows; ++row) {
            int hits = 0;
            const auto scanlineBB = SDL_FRect(0, row * heightPerTile, boardSizeX, heightPerTile);
            for (const auto &tileBB: tileBBs) {
                SDL_FRect intersection{};
                if (SDL_GetRectIntersectionFloat(&tileBB, &scanlineBB, &intersection)) {
                    if (intersection.h == heightPerTile) {
                        hits++;
                    }
                }
            }
            // Number of collisions with scanline geq to number of tiles per row
            // -> Entire line covered, schedule for deletion
            if (hits >= tileCols) {
                SDL_Log("Time to clear line %i", row);
                rowsToClear.push_back(row);
            }
        }
        if (rowsToClear.size() > 0) {
            ClearLines(rowsToClear);
        }
    }

    // TODO: Should just store this in memory
    std::vector<SDL_FRect> Board::GetBoardBoundingBoxes() {
        std::vector<SDL_FRect> bbs;
        bbs.reserve(4);
        // Floor
        bbs.emplace_back(0.0f, boardSizeY, boardSizeX, 10.0f);
        // Ceiling
        bbs.emplace_back(0.0f, -10.0f, boardSizeX, 9.9f);
        // Left side
        bbs.emplace_back(-10.0f, 0.0f, 10.0f, boardSizeY);
        // Right side
        bbs.emplace_back(boardSizeX, 0.0f, 10.0f, boardSizeY);
        return bbs;
    }

    void Board::CalculateCollisions() {
        m_collisions.clear();
        if (m_activeShape == nullptr) {
            // We're only interested in collisions with for the active shape
            return;
        }
        const auto activeShapeBBs = m_activeShape->GetCollisionBBs();
        for (const auto &bb: activeShapeBBs) {
            // Check collisions with frozen tiles
            for (const auto &tile: m_tiles) {
                SDL_FRect intersection{};
                const SDL_FRect tileBB = tile->BB();
                if (SDL_GetRectIntersectionFloat(bb.get(), &tileBB, &intersection)) {
                    // Register collision to be handled in next process step
                    m_collisions.emplace_back(Collision{*bb, tileBB, intersection});
                }
            }

            // Check collisions with bounding boxes
            for (const auto &boardBB: GetBoardBoundingBoxes()) {
                SDL_FRect intersection{};
                if (SDL_GetRectIntersectionFloat(bb.get(), &boardBB, &intersection)) {
                    // Register collision to be handled in next process step
                    m_collisions.emplace_back(Collision{*bb, boardBB, intersection});
                }
            }
        }
    }

    void Board::ProcessCollisions() {
        for (const auto &collision: m_collisions) {
            // Check position of intersection relative to position
            Math::Vec2 collisionDirection;
            if (collision.intersection.w >= 2.0f) {
                // Vertical collision
                if (collision.a.y >= collision.intersection.y) {
                    // Collision on the top side of the shape
                    collisionDirection.e[1] = -1;
                } else {
                    // Collision on the bottom side of the shape
                    collisionDirection.e[1] = 1;
                }
            }
            if (collision.intersection.h >= 2.0f) {
                // Horizontal collision
                if (collision.a.x >= collision.intersection.x) {
                    // Collision on the left side
                    collisionDirection.e[0] = -1;
                } else {
                    // Collision on the right side
                    collisionDirection.e[0] = 1;
                }
            }

            if (collisionDirection.y() != 0) {
                // Vertical collision
                // Stop movement of active shape
                m_activeShape->Freeze();

                // Collision on top side of the shape -> Game over
                if (collisionDirection.y() < 0) {
                    m_gameOver = true;
                }
            }
            if (collisionDirection.x() != 0) {
                // Horizontal collision -> Restrict input movement
                m_leftPressed = m_leftPressed && collisionDirection.x() > 0;
                m_rightPressed = m_rightPressed && collisionDirection.x() < 0;
            }
        }
    }

    Board::Board() {
        // Initialize shape configurations
        m_shapeConfigurations.reserve(3);
        // I
        ShapeConfiguration iConfig;
        iConfig.tilePositions.emplace_back(0, -48);
        iConfig.tilePositions.emplace_back(0, -16);
        iConfig.tilePositions.emplace_back(0, 16);
        iConfig.tilePositions.emplace_back(0, 48);
        m_shapeConfigurations.push_back(iConfig);
        // L
        ShapeConfiguration config;
        config.tilePositions.emplace_back(0, -48);
        config.tilePositions.emplace_back(0, -16);
        config.tilePositions.emplace_back(0, 16);
        config.tilePositions.emplace_back(-32, 16);
        m_shapeConfigurations.push_back(config);
        // T
        ShapeConfiguration tConfig;
        tConfig.tilePositions.emplace_back(0, -16);
        tConfig.tilePositions.emplace_back(0, 16);
        tConfig.tilePositions.emplace_back(-32, 16);
        tConfig.tilePositions.emplace_back(32, 16);
        m_shapeConfigurations.push_back(tConfig);

        AddRandomShape();
    }

    void Board::AddRandomShape() {
        // Pick random shape
        const int shapeIdx = SDL_rand(m_shapeConfigurations.size());
        const auto shape = m_shapeConfigurations[shapeIdx];

        // Probably dosent have to be shared
        m_activeShape = std::make_shared<Shape>(tileCols / 2 * widthPerTile, heightPerTile * 2, shape.tilePositions);
    }

    void Board::DrawGrid(SDL_Renderer *renderer) {
        // NOTE: Optimization: We could just calculate and store the grid data. This never changes
        // Even better: Shader :)
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
        } else if (e.key == MoveUp) {
            m_upPressed = true;
        } else if (e.key == RotateLeft) {
            if (m_activeShape != nullptr) {
                m_activeShape->Rotate(-M_PI / 2);
            }
        } else if (e.key == RotateRight) {
            if (m_activeShape != nullptr) {
                m_activeShape->Rotate(M_PI / 2);
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
        } else if (e.key == MoveUp) {
            m_upPressed = false;
        }
    }

    void Board::Draw(SDL_Renderer *renderer) {
        if (m_gameOver) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDebugText(renderer, boardSizeX / 2, boardSizeY / 2, "Game Over!");
            return;
        }
        DrawGrid(renderer);
        if (m_activeShape != nullptr) {
            m_activeShape->Draw(renderer);
        }
        for (const std::shared_ptr<Tile> &tile: m_tiles) {
            tile->Draw(renderer);
        }
    }

    void Board::Tick(const float dt) {
        CalculateCollisions();
        ProcessCollisions();

        if (m_activeShape != nullptr) {
            if (m_activeShape->IsGrounded() && !m_gameOver) {
                // Move tiles
                const auto tiles = m_activeShape->GetTiles();
                const Math::Mat3 shapeTransform = m_activeShape->GetTransform();
                m_tiles.reserve(m_tiles.size() + 4);
                for (const auto &tile: tiles) {
                    // Transform tile position to global pos
                    const Math::Vec3 globalPos = shapeTransform * Math::Vec3(tile->position.x(), tile->position.y(), 1);
                    // TODO: No need for a shared pointer
                    m_tiles.emplace_back(std::make_shared<Tile>(globalPos.x(), globalPos.y()));
                }
                m_activeShape.reset();
                // NOTE: Check if we have a memory leak here

                CheckForClearedLines();

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
                // Up just for debugging purposes
                //if (m_upPressed)
                //    inputVelocity += Math::Vec2(0, -1);
                m_activeShape->AddInputVelocity(inputVelocity);
            }
        }

        // Calculate frame for all subobjects
        if (m_tickTimer != nullptr) {
            m_tickTimer->Tick(dt);
        }
        if (m_activeShape != nullptr) {
            m_activeShape->Tick(dt);
        }
    }
} // Tetris
