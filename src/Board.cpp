//
// Created by lucas on 23.05.25.
//

#include "Board.h"

#include "Keymap.h"
#include "Constants.h"
#include "Benchmark/Instrumentor.h"
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
            for (const auto &tile: m_tiles) {
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
        m_score += rowIndices.size() * tileCols;
    }

    void Board::CheckForClearedLines() {
        PROFILE_FUNCTION();
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
        if (!rowsToClear.empty()) {
            ClearLines(rowsToClear);
        }
    }

    std::vector<SDL_FRect> Board::GetBoardBoundingBoxes() {
        std::vector<SDL_FRect> bbs;
        bbs.reserve(4);
        // Floor
        bbs.emplace_back(-100, boardSizeY, boardSizeX + 200.0f, 100.0f);
        // Ceiling
        bbs.emplace_back(-100.0f, -100.0f, boardSizeX + 200.0f, 99);
        // Left side
        bbs.emplace_back(-100.0f, -100.0f, 100, boardSizeY + 200.0f);
        // Right side
        bbs.emplace_back(boardSizeX, -100.0f, 100.0f, boardSizeY + 200.0f);
        return bbs;
    }

    void Board::CalculateCollisions(std::vector<Collision> &collisions) const {
        PROFILE_FUNCTION();
        collisions.clear();
        if (m_activeShape == nullptr) {
            // We're only interested in collisions for the active shape
            return;
        }
        const auto activeShapeBBs = m_activeShape->GetCollisionBBs();
        for (const auto &bb: activeShapeBBs) {
            // Check collisions with frozen tiles
            for (const auto &tile: m_tiles) {
                SDL_FRect intersection{};
                const SDL_FRect tileBB = tile->BB();
                if (SDL_GetRectIntersectionFloat(&bb, &tileBB, &intersection)) {
                    // Register collision to be handled in next process step
                    collisions.emplace_back(Collision{bb, tileBB, intersection});
                }
            }

            // Check collisions with bounding boxes
            for (const auto &boardBB: m_boardBBs) {
                SDL_FRect intersection{};
                if (SDL_GetRectIntersectionFloat(&bb, &boardBB, &intersection)) {
                    // Register collision to be handled in next process step
                    collisions.emplace_back(Collision{bb, boardBB, intersection});
                }
            }
        }
    }

    void Board::ProcessCollisions() {
        PROFILE_FUNCTION();
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

        SetupRendering();

        // Initialize Board bounding boxes
        m_boardBBs = GetBoardBoundingBoxes();
        AddRandomShape();
    }

    Board::~Board() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

    void Board::SetupRendering() {
        constexpr unsigned int indices[] = {0, 1, 2, 0, 2, 3};

        // Setup buffers
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);
        glBindVertexArray(m_vao);

        // Load shaders
        m_shader = Renderer::LoadShader("src/Shaders/rect.vert", "src/Shaders/gridv2.frag");
        // Bind vertex data
        constexpr float boardWidth = widthPerTile * tileCols;
        constexpr float boardHeight = heightPerTile * tileRows;
        constexpr float boardBoundaries[] = {
            0, 0, // TL
            boardWidth, 0, // TR
            boardWidth, boardHeight, // BR
            0, boardHeight, // BL
        };
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(boardBoundaries), boardBoundaries, GL_STATIC_DRAW);

        // Bind index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Bind position attribute
        const auto posLocation = glGetAttribLocation(m_shader, "pos");
        glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(posLocation);
    }

    void Board::AddRandomShape() {
        // Pick random shape
        const int shapeIdx = SDL_rand(m_shapeConfigurations.size());
        const auto shape = m_shapeConfigurations[shapeIdx];

        m_activeShape = std::make_unique<Shape>(tileCols / 2 * widthPerTile, heightPerTile * 2, shape.tilePositions);
    }

    void Board::AttemptRotation(const float &rotation) const {
        // (Temporarily) rotate shape
        m_activeShape->Rotate(rotation);

        // Update transform via 0s tick
        m_activeShape->Tick(0.0);

        // Check collisions
        std::vector<Collision> collisions;
        CalculateCollisions(collisions);

        if (!collisions.empty()) {
            // Rotate back if collided
            SDL_Log("Reverting");
            m_activeShape->Rotate(-rotation);
            // Update transform via 0s tick
            m_activeShape->Tick(0.0);
        }
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
                AttemptRotation(-M_PI / 2);
            }
        } else if (e.key == RotateRight) {
            if (m_activeShape != nullptr) {
                AttemptRotation(M_PI / 2);
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
        // Draw game over message
        if (m_gameOver) {
            // TODO: Needs fonts
            // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            // SDL_RenderDebugText(renderer, boardSizeX / 2, boardSizeY / 2, "Game Over!");
            return;
        }
        DrawGrid();
        if (m_activeShape != nullptr) {
            m_activeShape->Draw(renderer);
        }
        DrawTiles();
        // TODO: Score rendering needs fonts
        // DrawScore(renderer);
    }

    void Board::DrawTiles() const {
        std::vector<SDL_FRect> tiles;
        tiles.reserve(m_tiles.size());
        for (const auto &tile: m_tiles) {
            tiles.emplace_back(tile->BB());
        }
        Renderer::DrawSDLRects(tiles.data(), tiles.size());
    }

    void Board::DrawScore(SDL_Renderer *renderer) const {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDebugTextFormat(renderer, boardSizeX + 50, 50, "Score: %i", m_score);
    }

    void Board::DrawGrid() const {
        // Bind shader
        glUseProgram(m_shader);
        Renderer::CheckGLError("glUseProgram");
        // Bind mvp uniform
        const auto mvpLocation = glGetUniformLocation(m_shader, "mvp");
        glUniformMatrix4fv(mvpLocation, 1, false, boardMvp);
        Renderer::CheckGLError("glUniformMatrix4fv");
        // Bind vertex attributes
        glBindVertexArray(m_vao);
        Renderer::CheckGLError("glBindVertexArray");
        // Draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        Renderer::CheckGLError("glDrawElements");
        glBindVertexArray(0);

        // Debug: Draw bounding boxes
        constexpr std::array<float, 4> boundingBoxesColor = {1,0,0,1};
        Renderer::DrawSDLRects(m_boardBBs.data(), m_boardBBs.size(), boundingBoxesColor);
    }


    void Board::Tick(const float dt) {
        PROFILE_FUNCTION();
        CalculateCollisions(m_collisions);
        ProcessCollisions();

        if (m_activeShape != nullptr) {
            if (m_activeShape->IsGrounded() && !m_gameOver) {
                // Active shape has hit the ground
                // Move tiles
                const auto tiles = m_activeShape->GetTiles();
                const Math::Mat3 shapeTransform = m_activeShape->GetTransform();
                m_tiles.reserve(m_tiles.size() + 4);
                for (const auto &tile: tiles) {
                    // Transform tile position to global pos
                    const Math::Vec3 globalPos = shapeTransform * Math::Vec3(tile->position.x(), tile->position.y(), 1);
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
