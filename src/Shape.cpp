//
// Created by lucas on 23.05.25.
//

#include "Shape.h"

#include <cmath>

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

    void Shape::Draw(SDL_Renderer *renderer) {
        // Draw tiles
        for (const auto &tile: m_tiles) {
            tile->Draw(renderer);
        }
    }

    void Shape::Tick(const float dt) {
        if (m_grounded) {
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
        // Translation
        const float discreteY = std::round(m_position.y() / heightPerTile) * heightPerTile;
        const float discreteX = std::round(m_position.x() / widthPerTile) * widthPerTile;
        // Column-major order
        const Math::Mat3 translationMat = Math::Mat3(
            1, 0, 0,
            0, 1, 0,
            discreteX, discreteY, 1
        );

        // Rotation
        const float theta = 0.0f;
        const Math::Mat3 rotationMat = Math::Mat3(
            std::cos(theta), std::sin(theta), 0,
            -std::sin(theta), std::cos(theta), 0,
            0, 0, 1
        );
        return translationMat * rotationMat;
    }
} // Tetris
