//
// Created by lucas on 23.05.25.
//

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>

#include "SDL3/SDL_events.h"

#include "GameObject.h"
#include "Shape.h"
#include "TickTimer.h"

struct Collision {
    std::shared_ptr<Tetris::Shape> a;
    std::shared_ptr<Tetris::Shape> b;
    Math::Vec2 direction;
};

namespace Tetris {
    class Board : public GameObject {
    private:
        std::vector<std::shared_ptr<Shape>> m_shapes;
        std::vector<Collision> m_collisions;
        std::unique_ptr<TickTimer> m_tickTimer;

        std::shared_ptr<Shape> m_activeShape;

        // Keyboard control
        bool m_leftPressed{false};
        bool m_rightPressed{false};
        bool m_downPressed{false};
        bool m_gameOver{false};

        void CalculateCollisions();

        void ProcessCollisions();

        void AddRandomShape();

        static void DrawGrid(SDL_Renderer *renderer);
    public:
        Board();

        void HandleKeyDown(const SDL_KeyboardEvent&);
        void HandleKeyUp(const SDL_KeyboardEvent&);

        void Draw(SDL_Renderer* renderer) override;

        void Tick(float dt) override;
    };
} // Tetris

#endif //BOARD_H
