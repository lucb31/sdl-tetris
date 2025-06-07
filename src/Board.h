//
// Created by lucas on 23.05.25.
//

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>

#include "SDL3/SDL_events.h"

#include "GameObject.h"
#include "Renderer.h"
#include "Shape.h"
#include "TickTimer.h"

struct Collision {
    SDL_FRect a;
    SDL_FRect b;
    SDL_FRect intersection;
};

namespace Tetris {
    // Rendering constants
    constexpr float boardMvp[] = {
        2.0f / kScreenWidth, 0, 0, 0,
        0, -2.0f / kScreenHeight, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1,
    };

    struct ShapeConfiguration {
        std::vector<Math::Vec2> tilePositions;

        ShapeConfiguration() {
            tilePositions.reserve(4);
        }
    };

    class Board : public GameObject {
    private:
        // Game state
        bool m_gameOver{false};
        int m_score{0};
        // Needs to be shared to avoid copies in line clear method
        std::vector<std::shared_ptr<Tile> > m_tiles;
        std::unique_ptr<Shape> m_activeShape;
        std::vector<Collision> m_collisions;
        std::unique_ptr<TickTimer> m_tickTimer;

        // Board setup
        std::vector<ShapeConfiguration> m_shapeConfigurations;
        std::vector<SDL_FRect> m_boardBBs;

        // Rendering
        GLuint m_shader{}, m_ebo{}, m_vao{}, m_vbo{};

        // Keyboard control
        bool m_leftPressed{false};
        bool m_rightPressed{false};
        bool m_downPressed{false};
        bool m_upPressed{false};

        void ClearLines(const std::vector<int> &rowIndices);

        void CheckForClearedLines();

        static std::vector<SDL_FRect> GetBoardBoundingBoxes();

        void CalculateCollisions(std::vector<Collision> &collisions) const;

        void ProcessCollisions();

        void AddRandomShape();

        void AttemptRotation(const float &rotation) const;

        void DrawGrid() const;
        void DrawTiles() const;
        void DrawScore(SDL_Renderer *renderer) const;

    public:
        Board();
        ~Board();

        void SetupRendering();

        void HandleKeyDown(const SDL_KeyboardEvent &);

        void HandleKeyUp(const SDL_KeyboardEvent &);

        void Draw(SDL_Renderer *renderer) override;

        void Tick(float dt) override;
    };
} // Tetris

#endif //BOARD_H
