//
// Created by lucas on 27.07.25.
//

#ifndef SHAPEQUEUE_H
#define SHAPEQUEUE_H
#include <queue>
#include <vector>

#include "ShapeConfiguration.h"

namespace Tetris {
    class ShapeQueue {
        std::deque<ShapeConfiguration> m_queue;
        std::vector<ShapeConfiguration> m_shapePool;

    public:
        void Draw() const;
        void EnqueueRandom();

        ShapeConfiguration Next();

        ShapeQueue();
    };
} // Tetris

#endif //SHAPEQUEUE_H
