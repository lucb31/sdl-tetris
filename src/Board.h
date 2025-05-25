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
    SDL_FRect intersection;
};

namespace Tetris {
    class Board : public GameObject {
    private:
        std::vector<std::shared_ptr<Shape>> m_shapes;
        std::vector<Collision> m_collisions;
        std::unique_ptr<TickTimer> m_tickTimer;

        std::shared_ptr<Shape> m_activeShape;

        // Keyboard control
        bool m_leftPressed;
        bool m_rightPressed;

        void CalculateCollisions();

        void ProcessCollisions() const;

        void AddRandomShape();
        bool AddShape(const std::shared_ptr<Shape> &shape);
    public:
        Board();

        void HandleKeyDown(const SDL_KeyboardEvent&);
        void HandleKeyUp(const SDL_KeyboardEvent&);

        void Draw(SDL_Renderer* renderer) override;

        void Tick(float dt) override;
    };
} // Tetris

#endif //BOARD_H
