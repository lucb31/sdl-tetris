//
// Created by lucas on 27.07.25.
//

#include "ShapeQueue.h"

#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"

namespace Tetris {
    ShapeQueue::ShapeQueue() {
        // Initialize shape configurations
        m_shapePool.reserve(3);
        // I
        ShapeConfiguration iConfig;
        iConfig.tilePositions.emplace_back(0, -48);
        iConfig.tilePositions.emplace_back(0, -16);
        iConfig.tilePositions.emplace_back(0, 16);
        iConfig.tilePositions.emplace_back(0, 48);
        iConfig.color = glm::vec4(0, 1, 0, 1);
        iConfig.name = "I Shape";
        m_shapePool.push_back(iConfig);
        // L
        ShapeConfiguration config;
        config.tilePositions.emplace_back(0, -48);
        config.tilePositions.emplace_back(0, -16);
        config.tilePositions.emplace_back(0, 16);
        config.tilePositions.emplace_back(-32, 16);
        config.color = glm::vec4(1, 0, 0, 1);
        config.name = "L Shape";
        m_shapePool.push_back(config);
        // T
        ShapeConfiguration tConfig;
        tConfig.tilePositions.emplace_back(0, -16);
        tConfig.tilePositions.emplace_back(0, 16);
        tConfig.tilePositions.emplace_back(-32, 16);
        tConfig.tilePositions.emplace_back(32, 16);
        tConfig.color = glm::vec4(0, 0, 1, 1);
        tConfig.name = "T Shape";
        m_shapePool.push_back(tConfig);

        // Fill Queue with 3 random shapes
        for (int i = 0; i < 3; i++) {
            EnqueueRandom();
        }
    }

    void ShapeQueue::EnqueueRandom() {
        const int shapeIdx = SDL_rand(m_shapePool.size());
        m_queue.push(m_shapePool[shapeIdx]);
        SDL_Log("Pushed %s", m_shapePool[shapeIdx].name.c_str());
    }

    ShapeConfiguration ShapeQueue::Next() {
        if (m_queue.empty()) {
            SDL_Log("Trying to access an empty queue");
            return ShapeConfiguration{};
        }
        const auto config = m_queue.front();
        SDL_Log("Popped %s", config.name.c_str());
        m_queue.pop();
        // Enqueue a new random shape to ensure there's always 3 shapes
        EnqueueRandom();
        return config;
    }
} // Tetris
