//
// Created by lucas on 23.05.25.
//

#ifndef SHAPE_H
#define SHAPE_H
#include "GameObject.h"
#include "Vec2.h"

namespace Tetris {

class Shape : public GameObject {
private:
    bool m_grounded;
    Math::Vec2 m_position;
    Math::Vec2 m_velocity;

    public:
    Shape(float x, float y);
    void Draw(SDL_Surface *) override;
    void Tick(float dt) override;
};

} // Tetris

#endif //SHAPE_H
