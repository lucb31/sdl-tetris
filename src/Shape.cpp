//
// Created by lucas on 23.05.25.
//

#include "Shape.h"

#include <cmath>

#include "Constants.h"

namespace Tetris {
    void Shape::CalculateVelocity() {
        m_velocity = m_inputVelocity + Math::Vec2(0, m_gravity);
    }

    void Shape::Freeze() {
        m_velocity = Math::Vec2();
        m_grounded = true;
    }

    void Shape::AddInputVelocity(const Math::Vec2 &direction) {
        if (m_grounded) {
            return;
        }
        Math::Vec2 normalizedDirection = direction.Norm();
        m_inputVelocity = normalizedDirection * m_speed;
    }

    void Shape::Rotate(float radians) {
        m_rotation += radians;
    }

    void Shape::Draw(SDL_Renderer *renderer) {
        // Draw tiles
        for (const auto &tile: m_tiles) {
            tile->Draw(renderer);
        }

        // Draw shape center
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDebugText(renderer, m_position.x(), m_position.y(), "X");
    }

    void Shape::Tick(const float dt) {
        if (m_grounded) {
            // No need to re-calculate position, velocity, transform ...
            return;
        }
        CalculateVelocity();
        m_position += m_velocity * dt;

        const Math::Mat3 transform = GetTransform();
        for (const auto &tile: m_tiles) {
            // Pointer solution would be more elegant
            tile->parentTransform = transform;
        }

        // Collision with floor
        if (m_position.y() >= (tileRows - 1) * heightPerTile) {
            Freeze();
        }
    }

    std::vector<std::shared_ptr<SDL_FRect>> Shape::GetCollisionBBs() const {
        // Calculate BBs for all tiles
        std::vector<std::shared_ptr<SDL_FRect> > collisionBBs;
        collisionBBs.reserve(m_tiles.size());
        for (const auto &tile: m_tiles) {
            collisionBBs.emplace_back(std::make_unique<SDL_FRect>(tile->BB()));
        }
        return collisionBBs;
    }

    Math::Mat3 Shape::GetTransform() const {
        // Column-major order
        const Math::Mat3 translationMat = Math::Mat3(
            1, 0, 0,
            0, 1, 0,
            m_position.x(), m_position.y(), 1
        );

        // Rotation
        const Math::Mat3 rotationMat = Math::Mat3(
            std::cos(m_rotation), std::sin(m_rotation), 0,
            -std::sin(m_rotation), std::cos(m_rotation), 0,
            0, 0, 1
        );
        return translationMat * rotationMat;
    }

    Shape::Shape(const float x, const float y, const std::vector<Math::Vec2>& tilePositions) : m_position(Math::Vec2(x, y)), m_velocity(Math::Vec2(0, 0)) {
        // Initialize tiles
        m_tiles.reserve(tilePositions.size());
        const auto t = GetTransform();
        for (int i = 0; i < tilePositions.size(); i++) {
            m_tiles.emplace_back(std::make_unique<Tile>(tilePositions[i]));
            m_tiles[i]->parentTransform = t;
        }
    }
} // Tetris
