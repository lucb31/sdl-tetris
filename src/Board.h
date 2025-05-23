//
// Created by lucas on 23.05.25.
//

#ifndef BOARD_H
#define BOARD_H
#include "Shape.h"

#include <vector>
#include <memory>

#include "GameObject.h"
#include "SDL3/SDL_surface.h"

namespace Tetris {

class Board : public GameObject {
private:
    std::vector<std::shared_ptr<Shape>> m_shapes;

    public:
    bool AddShape(Shape *shape);
    void Draw(SDL_Surface *surf) override;
    void Tick(float dt) override;
};

} // Tetris

#endif //BOARD_H
