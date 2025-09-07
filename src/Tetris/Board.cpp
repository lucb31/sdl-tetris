//
// Created by lucas on 23.05.25.
//

#include "Board.h"

#include "Keymap.h"
#include "Constants.h"
#include "../Benchmark/Instrumentor.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.inl"
#include "SDL3/SDL_log.h"

namespace Tetris {
    void Board::ClearLines(const std::vector<int> &rowIndices) {
        for (const auto &rowIndex: rowIndices) {
            // Separate list of survivors that will replace the
            // list of tiles after every row iteration
            std::vector<std::shared_ptr<Tile> > survivors;
            survivors.reserve(m_staticTiles.size() - tileCols);
            int survive = 0;
            int moveDown = 0;
            int remove = 0;
            const int total = m_staticTiles.size();

            // Bounds for the current row
            const float rowMinY = m_position.y + rowIndex * heightPerTile;
            const float rowMaxY = m_position.y + (rowIndex + 1) * heightPerTile;
            for (const auto &tile: m_staticTiles) {
                // Use bb here to get GLOBAL position of tile, not local
                const auto tileBB = tile->BB();
                if (tileBB.y >= rowMaxY) {
                    // If below row -> Survives
                    survivors.push_back(tile);
                    survive++;
                } else if (tileBB.y < rowMinY) {
                    // If above row -> Survive & Move down one row
                    tile->position += glm::vec2(0.0f, (float) heightPerTile);
                    survivors.push_back(tile);
                    moveDown++;
                } else {
                    remove++;
                    // Will be removed, by not adding it to the survivors
                }
            }
            // Update tiles with survivor list
            m_staticTiles = survivors;
            SDL_Log("From %i total tiles %i will stay in place, %i will move down and %i will be removed", total,
                    survive, moveDown, remove);
        }
        m_score += rowIndices.size() * tileCols;
        UpdateScore();
    }

    void Board::CheckForClearedLines() {
        PROFILE_FUNCTION();
        // Retrieve list of all frozen tile bounding boxes
        std::vector<SDL_FRect> tileBBs;
        tileBBs.reserve(m_staticTiles.size());
        for (const auto &tile: m_staticTiles) {
            tileBBs.push_back(tile->BB());
        }

        // Iterate over every scanline and check for collisions with tile BBs
        std::vector<int> rowsToClear;
        for (int row = 0; row < tileRows; ++row) {
            int hits = 0;
            const auto scanlineBB = SDL_FRect(m_position.x, m_position.y + row * heightPerTile, boardSizeX,
                                              heightPerTile);
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
                SDL_Log("Time to clear line %i / %i", row, tileRows);
                rowsToClear.push_back(row);
            }
        }
        if (!rowsToClear.empty()) {
            ClearLines(rowsToClear);
        }
    }

    std::vector<SDL_FRect> Board::GetBoardBoundingBoxes() const {
        std::vector<SDL_FRect> bbs;
        bbs.reserve(4);
        // Floor
        bbs.emplace_back(m_position.x - 10.0f, m_position.y + boardSizeY, boardSizeX + 20.0f, 10.0f);
        // Ceiling
        bbs.emplace_back(m_position.x - 10.0f, m_position.y - 10.0f, boardSizeX + 20.0f, 10.0f);
        // Left side
        bbs.emplace_back(m_position.x - 10.0f, m_position.y - 10.0f, 10.0f, boardSizeY + 20.0f);
        // Right side
        bbs.emplace_back(m_position.x + boardSizeX, m_position.y - 10.0f, 10.0f, boardSizeY + 20.0f);
        return bbs;
    }

    void Board::SetupProjection() {
        // Initialize board position
        constexpr float boardOffsetX = (float) (kScreenWidth - tileCols * widthPerTile) / 2.0f;
        constexpr float boardOffsetY = (float) (kScreenHeight - tileRows * heightPerTile) / 2.0f;
        m_position = glm::vec2(boardOffsetX, boardOffsetY);
        const auto boardTranslation = glm::translate(glm::mat4(1), glm::vec3(m_position, 0.0f));

        // Initialize projection & mvp
        m_projection = glm::ortho(0.0f, (float) kScreenWidth, (float) kScreenHeight, 0.0f);
        m_mvp = m_projection * boardTranslation;
    }

    Board::Board() {
        SetupRendering();
        SetupProjection();

        // Initialize UI strings
        m_gameOverString = StringRenderer();
        m_gameOverString.SetPosition(glm::vec2(kScreenWidth / 2, kScreenHeight / 2));
        m_gameOverString.SetString("Game Over");
        m_scoreString = StringRenderer();
        m_scoreString.SetPosition(glm::vec2(m_position.x + boardSizeX + 50, m_position.y + 50));
        m_highScoreString = StringRenderer();
        m_highScoreString.SetPosition(glm::vec2(m_position.x + boardSizeX + 50, m_position.y + 100));

        // Initialize Board bounding boxes
        m_boardBBs = GetBoardBoundingBoxes();

        ResetBoard();
    }

    void Board::ResetBoard() {
        m_staticTiles = std::vector<std::shared_ptr<Tile> >();
        m_queue.reset();
        m_queue = std::make_unique<ShapeQueue>();
        m_gameOver = false;
        m_score = 0;
        UpdateScore();
        AddRandomShape();
    }

    void Board::UpdateScore() {
        m_scoreString.SetString(std::format("Score: {}", m_score));
        m_highScore = std::max(m_score, m_highScore);
        m_highScoreString.SetString(std::format("HighScore: {}", m_highScore));
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
        m_shader = Renderer::LoadShader("rect.vert", "gridv2.frag");
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
        // Check if there's still space available. If any tile collides with the top row -> Game Over
        const SDL_FRect ceilingBB = SDL_FRect(m_position.x, m_position.y, boardSizeX, heightPerTile);
        for (const auto &tile: m_staticTiles) {
            const auto tileBB = tile->BB();
            SDL_FRect intersection{};
            if (SDL_GetRectIntersectionFloat(&tileBB, &ceilingBB, &intersection)) {
                SDL_Log("No more space available in top row");
                m_gameOver = true;
                break;
            }
        }
        if (m_gameOver) {
            SDL_Log("Aborting new shape");
            return;
        }

        // Need to pass all collision bbs to the shape so it can do collision checking
        // on its own. Ideally we would have a collision server managing this
        std::vector<SDL_FRect> bbs;
        bbs.reserve(m_boardBBs.size());
        bbs.insert(bbs.end(), m_boardBBs.begin(), m_boardBBs.end());
        for (const auto &tile: m_staticTiles) {
            bbs.emplace_back(tile->BB());
        }

        // Pop shape from queue and enqueue a new one
        const ShapeConfiguration shape = m_queue->Next();

        // + 0.5 to offset by half width; avoids rounding errors when moving by 1 tile width
        constexpr float offsetX = boardSizeX / 2.0f + 10.0f;
        constexpr float offsetY = heightPerTile * 2;
        const auto shapeSpawnPos = m_position + glm::vec2(offsetX, offsetY);
        m_activeShape = std::make_unique<Shape>(shapeSpawnPos, shape, bbs);
    }


    void Board::HandleKeyDown(const SDL_KeyboardEvent &e) {
        if (e.key == MoveLeft) {
            if (!m_leftPressed) m_leftJustPressed = true;
            m_leftPressed = true;
        } else if (e.key == MoveRight) {
            if (!m_rightPressed) m_rightJustPressed = true;
            m_rightPressed = true;
        } else if (e.key == MoveDown) {
            if (!m_downPressed) m_downJustPressed = true;
            m_downPressed = true;
        } else if (e.key == MoveUp) {
            m_upPressed = true;
        } else if (e.key == RotateLeft) {
            if (m_activeShape != nullptr) {
                m_activeShape->AttemptRotation(-M_PI / 2);
            }
        } else if (e.key == RotateRight) {
            if (m_activeShape != nullptr) {
                m_activeShape->AttemptRotation(M_PI / 2);
            }
        } else if (e.key == LockShape) {
            if (m_activeShape != nullptr) {
                m_activeShape->MarkToLock();
            }
        } else if (e.key == Restart) {
            if (m_gameOver) {
                ResetBoard();
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

    void Board::Draw() {
        DrawGrid();
        DrawStaticTiles();
        if (m_activeShape != nullptr) {
            m_activeShape->Draw();
        }
        DrawScore();
        m_queue->Draw();
        if (m_gameOver) {
            m_gameOverString.Render();
        }
    }

    void Board::DrawStaticTiles() const {
        std::vector<SDL_FRect> tiles;
        tiles.reserve(m_staticTiles.size());
        for (const auto &tile: m_staticTiles) {
            tiles.emplace_back(tile->BB());
        }
        Renderer::DrawSDLRects(tiles.data(), tiles.size());
    }

    void Board::DrawScore() {
        m_scoreString.Render();
        m_highScoreString.Render();
    }

    void Board::DrawGrid() const {
        // Bind shader
        glUseProgram(m_shader);
        Renderer::CheckGLError("glUseProgram");
        // Bind mvp uniform
        const auto mvpLocation = glGetUniformLocation(m_shader, "mvp");
        glUniformMatrix4fv(mvpLocation, 1, false, glm::value_ptr(m_mvp));
        Renderer::CheckGLError("glUniformMatrix4fv");
        // Bind vertex attributes
        glBindVertexArray(m_vao);
        Renderer::CheckGLError("glBindVertexArray");
        // Draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        Renderer::CheckGLError("glDrawElements");
        glBindVertexArray(0);

        // Debug: Draw bounding boxes
        constexpr std::array<float, 4> boundingBoxesColor = {1, 0, 0, 1};
        Renderer::DrawSDLRects(m_boardBBs.data(), m_boardBBs.size(), boundingBoxesColor);
    }


    void Board::Tick(const float dt) {
        if (m_gameOver) {
            // Nothing to do
            return;
        }
        PROFILE_FUNCTION();

        if (m_activeShape != nullptr) {
            if (m_activeShape->IsGrounded()) {
                // Active shape has hit the ground
                // Move shape tiles to static board tiles
                const auto tiles = m_activeShape->GetTiles();
                const glm::mat4 shapeTransform = m_activeShape->GetTransform();
                m_staticTiles.reserve(m_staticTiles.size() + 4);
                for (const auto &tile: tiles) {
                    // Transform tile position to global pos
                    const glm::vec4 globalPos = shapeTransform * glm::vec4(tile->position, 1.0f, 1.0f);
                    m_staticTiles.emplace_back(std::make_shared<Tile>(globalPos));
                }
                m_activeShape.reset();

                CheckForClearedLines();

                // Set timer to spawn next shape
                m_shapeSpawnTimer = std::make_unique<TickTimer>(0.2f, [this](int) {
                    AddRandomShape();
                });
            } else {
                // Apply active shape movement inputs
                auto inputVelocity = glm::vec2(0.0f);
                if (m_leftJustPressed)
                    inputVelocity += glm::vec2(-1, 0);
                if (m_rightJustPressed)
                    inputVelocity += glm::vec2(1, 0);
                if (m_downJustPressed)
                    inputVelocity += glm::vec2(0, 1);
                // Up just for debugging purposes
                if (m_upPressed)
                    inputVelocity += glm::vec2(0, -1);
                m_activeShape->AddInputVelocity(inputVelocity);
            }
        }

        // Calculate frame for all subobjects
        if (m_shapeSpawnTimer != nullptr) {
            m_shapeSpawnTimer->Tick(dt);
        }
        if (m_activeShape != nullptr) {
            m_activeShape->Tick(dt);
        }
        m_rightJustPressed = false;
        m_leftJustPressed = false;
        m_downJustPressed = false;
    }
} // Tetris
