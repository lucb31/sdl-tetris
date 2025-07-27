//
// Created by lucas on 27.07.25.
//

#include "ShapeQueue.h"

#include "Constants.h"
#include "Shape.h"
#include "../Renderer.h"

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

    // Dirty solution just to get something displayed.
    // Problem: Tile / shape rendering does not support scaling well
    // Solution: Probably best to have a simple 'ShapePreview' class
    void ShapeQueue::Draw() const {
        // Draw containers
        std::vector<SDL_FRect> rects;
        rects.reserve(m_queue.size());
        rects.emplace_back(200, 100, 100, 100);
        rects.emplace_back(212.5, 225, 75, 75);
        rects.emplace_back(212.5, 325, 75, 75);
        Renderer::DrawSDLFRectsOutline(rects.data(), rects.size());

        // Draw previews
        std::vector<SDL_FRect> outsideBBs;
        int i = 0;
        for (const auto config: m_queue) {
            float scale = 0.5f;
            if (i == 0) scale = 0.75f;
            auto shape = Shape(
                rects.at(i).x + widthPerTile * 2 * scale,
                rects.at(i).y + heightPerTile * 2 * scale,
                config, outsideBBs
            );
            shape.SetScale(glm::vec2(scale));
            shape.DrawPreview();
            i++;
        }
    }

    void ShapeQueue::EnqueueRandom() {
        const int shapeIdx = SDL_rand(m_shapePool.size());
        m_queue.push_back(m_shapePool[shapeIdx]);
    }

    ShapeConfiguration ShapeQueue::Next() {
        if (m_queue.empty()) {
            SDL_Log("Trying to access an empty queue");
            return ShapeConfiguration{};
        }
        const auto config = m_queue.front();
        m_queue.pop_front();
        // Enqueue a new random shape to ensure there's always 3 shapes
        EnqueueRandom();
        return config;
    }
} // Tetris
