//
// Created by lucas on 23.05.25.
//

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void Draw() = 0;

    virtual void Tick(float dt) = 0;
};
#endif //GAMEOBJECT_H
