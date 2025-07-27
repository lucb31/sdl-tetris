//
// Created by lucas on 23.05.25.
//

#include "Board.h"

#include "Keymap.h"
#include "Constants.h"
#include "Benchmark/Instrumentor.h"
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
            survivors.reserve(m_tiles.size() - tileCols);

            // Bounds for the current row
            const float rowMinY = m_position.y + rowIndex * heightPerTile;
            const float rowMaxY = m_position.y + (rowIndex + 1) * heightPerTile;
            for (const auto &tile: m_tiles) {
                // Use bb here to get GLOBAL position of tile, not local
                const auto tileBB = tile->BB();
                if (tileBB.y > rowMaxY) {
                    // If below row -> Survives
                    survivors.push_back(tile);
                } else if (tileBB.y < rowMinY) {
                    // If above row -> Survive & Move down one row
                    tile->position += glm::vec2(0, heightPerTile);
                    survivors.push_back(tile);
                } else {
                    // Will be removed, by not adding it to the survivors
                }
            }
            // Update tiles with survivor list
            m_tiles = survivors;
        }
        m_score += rowIndices.size() * tileCols;
        m_scoreString.SetString(std::format("Score: {}", m_score));
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
            const auto scanlineBB = SDL_FRect(m_position.x, m_position.y + row * heightPerTile, boardSizeX, heightPerTile);
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
        SetupProjection();

        // Initialize UI strings
        m_gameOverString = StringRenderer();
        m_gameOverString.SetPosition(glm::vec2(kScreenWidth / 2, kScreenHeight / 2));
        m_gameOverString.SetString("Game Over");
        m_scoreString = StringRenderer();
        m_scoreString.SetPosition(glm::vec2(boardSizeX + 50, 50));
        m_scoreString.SetString("Score: 0");

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
        // Check if there's still space available. If any tile collides with the top row -> Game Over
        const SDL_FRect ceilingBB = SDL_FRect(m_position.x, m_position.y, boardSizeX, heightPerTile);
        for (const auto &tile: m_tiles) {
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

        // Pick random shape
        const int shapeIdx = SDL_rand(m_shapeConfigurations.size());
        const auto shape = m_shapeConfigurations[shapeIdx];

        // Need to pass all collision bbs to the shape so it can do collision checking
        // on its own. Ideally we would have a collision server managing this
        std::vector<SDL_FRect> bbs;
        bbs.reserve(m_boardBBs.size());
        bbs.insert(bbs.end(), m_boardBBs.begin(), m_boardBBs.end());
        for (const auto &tile: m_tiles) {
            const SDL_FRect tileBB = tile->BB();
            bbs.push_back(tileBB);
        }

        // + 0.5 to offset by half width; avoids rounding errors when moving by 1 tile width
        const float offsetX = boardSizeX / 2.0f;
        const float offsetY = heightPerTile * 2;
        const auto shapeSpawnPos = m_position + glm::vec2(offsetX, offsetY);
        m_activeShape = std::make_unique<Shape>(shapeSpawnPos, shape.tilePositions, bbs);
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
        // Draw game over message
        if (m_gameOver) {
            m_gameOverString.Render();
        }
        if (m_activeShape != nullptr) {
            m_activeShape->Draw();
        }
        DrawTiles();
        DrawScore();
    }

    void Board::DrawTiles() const {
        std::vector<SDL_FRect> tiles;
        tiles.reserve(m_tiles.size());
        for (const auto &tile: m_tiles) {
            tiles.emplace_back(tile->BB());
        }
        Renderer::DrawSDLRects(tiles.data(), tiles.size());
    }

    void Board::DrawScore() {
        m_scoreString.Render();
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
                // Move tiles
                const auto tiles = m_activeShape->GetTiles();
                const glm::mat4 shapeTransform = m_activeShape->GetTransform();
                m_tiles.reserve(m_tiles.size() + 4);
                for (const auto &tile: tiles) {
                    // Transform tile position to global pos
                    const glm::vec4 globalPos = shapeTransform * glm::vec4(tile->position.x, tile->position.y, 1, 1);
                    m_tiles.emplace_back(std::make_shared<Tile>(globalPos.x, globalPos.y));
                }
                m_activeShape.reset();

                CheckForClearedLines();

                // Set timer to spawn next shape
                m_tickTimer = std::make_unique<TickTimer>(1.0f, [this](int) {
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
        if (m_tickTimer != nullptr) {
            m_tickTimer->Tick(dt);
        }
        if (m_activeShape != nullptr) {
            m_activeShape->Tick(dt);
        }
        m_rightJustPressed = false;
        m_leftJustPressed = false;
        m_downJustPressed = false;
    }
} // Tetris
