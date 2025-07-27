//
// Created by lucas on 23.05.25.
//

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>

#include "SDL3/SDL_events.h"

#include "../GameObject.h"
#include "../Renderer.h"
#include "Shape.h"
#include "ShapeConfiguration.h"
#include "ShapeQueue.h"
#include "../StringRenderer.h"
#include "../TickTimer.h"

namespace Tetris {
    class Board : public GameObject {
    private:
        // Game state
        bool m_gameOver{false};
        int m_score{0};
        // Needs to be shared to avoid copies in line clear method
        std::vector<std::shared_ptr<Tile>> m_tiles;
        std::unique_ptr<Shape> m_activeShape;
        // Used for timeout between shape spawns
        std::unique_ptr<TickTimer> m_tickTimer;
        std::unique_ptr<ShapeQueue> m_queue;

        std::vector<SDL_FRect> m_boardBBs;

        // UI
        StringRenderer m_gameOverString;
        StringRenderer m_scoreString;

        // Rendering
        GLuint m_shader{}, m_ebo{}, m_vao{}, m_vbo{};
        glm::vec2 m_position;
        glm::mat4 m_projection;
        glm::mat4 m_mvp;

        // Keyboard control
        bool m_leftPressed{false};
        bool m_leftJustPressed{false};
        bool m_rightPressed{false};
        bool m_rightJustPressed{false};
        bool m_downPressed{false};
        bool m_downJustPressed{false};
        bool m_upPressed{false};

        void ClearLines(const std::vector<int> &rowIndices);

        void CheckForClearedLines();

        std::vector<SDL_FRect> GetBoardBoundingBoxes() const;

        void SetupProjection();

        void AddRandomShape();

        void DrawGrid() const;

        void DrawTiles() const;

        void DrawScore();

        void SetupRendering();

    public:
        Board();
        ~Board();

        void HandleKeyDown(const SDL_KeyboardEvent &);

        void HandleKeyUp(const SDL_KeyboardEvent &);

        void Draw() override;

        void Tick(float dt) override;
    };
} // Tetris

#endif //BOARD_H
