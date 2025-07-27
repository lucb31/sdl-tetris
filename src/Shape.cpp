//
// Created by lucas on 23.05.25.
//

#include "Shape.h"

#include <glm/glm.hpp>

#include "Constants.h"
#include "Renderer.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Tetris {
    void Shape::CalculateVelocity() {
        m_velocity = m_inputVelocity + GRAVITY_VECTOR;
    }

    void Shape::Freeze() {
        m_velocity = glm::vec2(0.0f);
        m_grounded = true;
    }

    void Shape::AddInputVelocity(const glm::vec2 &direction) {
        if (m_grounded) {
            return;
        }
        if (glm::length(direction) > 0.005f) {
            m_inputVelocity = glm::normalize(direction) * (float)widthPerTile / 0.016f;
        } else {
            m_inputVelocity = glm::vec2(0.0f);
        }
    }

    void Shape::Rotate(float radians) {
        m_rotation += radians;
    }

    void Shape::Draw() {
        // Draw tiles
        std::vector<SDL_FRect> tiles;
        tiles.reserve(m_tiles.size());
        for (const auto &tile: m_tiles) {
            tiles.emplace_back(tile->BB());
        }
        Renderer::DrawSDLRects(tiles.data(), tiles.size());

        // Draw shape center
        //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        //SDL_RenderDebugText(renderer, m_position.x, m_position.y, "X");

        // Debug display shape position
        //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        //SDL_RenderDebugTextFormat(renderer, boardSizeX + 50, 100, "Position: (%.1f, %.1f)", m_position.x, m_position.y);
    }

    void Shape::Tick(const float dt) {
        if (m_grounded) {
            // No need to re-calculate position, velocity, transform ...
            return;
        }
        CalculateVelocity();
        m_position += m_velocity * dt;

        const glm::mat4 transform = GetTransform();
        for (const auto &tile: m_tiles) {
            // Pointer solution would be more elegant
            tile->parentTransform = transform;
        }
    }

    // Shared ptr probably not required here. Can just copy
    std::vector<SDL_FRect> Shape::GetCollisionBBs() const {
        // Calculate BBs for all tiles
        std::vector<SDL_FRect> collisionBBs;
        collisionBBs.reserve(m_tiles.size());
        for (const auto &tile: m_tiles) {
            collisionBBs.emplace_back(tile->BB());
        }
        return collisionBBs;
    }

    glm::mat4 Shape::GetTransform() const {
        const glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(m_position.x, m_position.y, 0.0f));
        const glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        return translate * rotate;
    }

    Shape::Shape(const float x, const float y,
                 const std::vector<glm::vec2> &tilePositions) : m_position(glm::vec2(x, y)),
                                                                m_velocity(glm::vec2(0.0f)) {
        // Initialize tiles
        m_tiles.reserve(tilePositions.size());
        const auto t = GetTransform();
        for (int i = 0; i < tilePositions.size(); i++) {
            m_tiles.emplace_back(std::make_shared<Tile>(tilePositions[i]));
            m_tiles[i]->parentTransform = t;
        }
    }
} // Tetris
