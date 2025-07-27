//
// Created by lucas on 23.05.25.
//

#include "Shape.h"

#include <glm/glm.hpp>

#include "Constants.h"
#include "../Renderer.h"
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
            m_inputVelocity = glm::normalize(direction) * (float) widthPerTile / 0.016f;
        } else {
            m_inputVelocity = glm::vec2(0.0f);
        }
    }

    void Shape::Rotate(float radians) {
        m_rotation += radians;
    }

    // Interims solution to draw a preview of the shape with scale
    void Shape::DrawPreview() const {
        std::vector<SDL_FRect> tiles;
        tiles.reserve(m_tiles.size());
        for (const auto &tile: m_tiles) {
            const glm::vec2 topLeft = tile->GetGlobalTopLeft();
            tiles.emplace_back(topLeft.x, topLeft.y, widthPerTile * m_scale.x, heightPerTile * m_scale.y);
        }
        Renderer::DrawSDLRects(tiles.data(), tiles.size(),
                               std::array{m_color.x, m_color.y, m_color.z, m_color.w});
    }

    void Shape::Draw() {
        // Draw tiles
        std::vector<SDL_FRect> tiles;
        tiles.reserve(m_tiles.size());
        for (const auto &tile: m_tiles) {
            tiles.emplace_back(tile->BB());
        }
        Renderer::DrawSDLRects(tiles.data(), tiles.size(),
                               std::array<float, 4>{m_color.x, m_color.y, m_color.z, m_color.w});

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
        if (m_shouldLock) {
            Lock(dt);
        } else {
            CalculateVelocity();
        }
        MoveAndSlide(dt);
    }

    void Shape::AttemptRotation(const float &rotation) {
        // (Temporarily) rotate shape
        Rotate(rotation);
        UpdateTransform();
        // Calculate collisions
        std::vector<Collision> collisions = CalculateCollisions();

        // Check for intersecting collisions
        bool needToRevert = false;
        if (!collisions.empty()) {
            for (const Collision &collision: collisions) {
                const float collisionArea = collision.intersection.h * collision.intersection.w;
                if (collisionArea > 0.0f) {
                    needToRevert = true;
                    break;
                }
            }
        }

        // Revert rotation movement if required
        if (needToRevert) {
            // Rotate back if collided
            SDL_Log("Detected intersecting collision after rotation. Reverting rotation.");
            Rotate(-rotation);
            UpdateTransform();
        }
    }

    void Shape::UpdateTransform() const {
        const glm::mat4 transform = GetTransform();
        for (const auto &tile: m_tiles) {
            // Pointer solution would be more elegant
            tile->parentTransform = transform;
        }
    }

    void Shape::MoveAndSlide(const float dt) {
        const glm::vec<2, float> oldPosition = m_position;
        m_position = m_position + m_velocity * dt;
        UpdateTransform();

        // Check collisions
        const auto collisions = CalculateCollisions();
        if (collisions.empty()) {
            return;
        }

        // Handle collisions
        auto realVelocity = m_velocity;
        for (const auto &collision: collisions) {
            const auto area = collision.intersection.w * collision.intersection.h;
            // Touching collision
            if (area < 0.1f) {
                // SDL_Log("Touching collision: Ignoring.");
            } else {
                SDL_Log("Intrusion collision detected.");
                // Default initialize normal in negative y direction -> Assuming a vertical collision
                glm::vec2 collisionNormal = glm::vec2(0.0f, -1.0f);
                if (glm::abs(m_velocity.x) > 0.0f) {
                    SDL_Log("You had some horizontal velocity. Assuming this was a horizontal collision");
                    collisionNormal = glm::vec2(glm::sign(m_velocity.x) * 1.0f, 0.0f);
                } else {
                    // We had no horizontal velocity and still collided -> Must be a vertical collision
                    SDL_Log("Vertical collision detected. Freezing....");
                    Freeze();
                }
                // Adjust real velocity to restrict any movement in direction of collision normal
                realVelocity = realVelocity - collisionNormal * glm::dot(realVelocity, collisionNormal);
            }
        }
        // Correct position
        m_position = oldPosition + realVelocity * dt;
        UpdateTransform();
    }

    std::vector<Collision> Shape::CalculateCollisions() const {
        std::vector<Collision> collisions;
        const auto activeShapeBBs = GetCollisionBBs();
        for (const auto &shapeBB: activeShapeBBs) {
            for (const auto &otherBB: m_outsideBBs) {
                SDL_FRect intersection{};
                if (SDL_GetRectIntersectionFloat(&shapeBB, &otherBB, &intersection)) {
                    collisions.emplace_back(Collision{shapeBB, otherBB, intersection});
                }
            }
        }
        return collisions;
    }

    void Shape::Lock(const float &dt) {
        // Determine minimum distance to the next bb
        float minDistanceY = 999999999.0f;
        constexpr auto lineDir = glm::vec2(0.0f, 1.0f);

        for (const auto &tile: m_tiles) {
            const auto topLeft = tile->GetGlobalTopLeft();
            const auto lineOrig = glm::vec2(
                topLeft.x + widthPerTile / 2.0f,
                topLeft.y + heightPerTile
            );

            for (const auto &otherBB: m_outsideBBs) {
                const auto bbP1 = glm::vec2(otherBB.x, otherBB.y);
                const auto bbP2 = glm::vec2(otherBB.x + otherBB.w, otherBB.y);
                const auto p = RayIntersectsSegmentAt(lineOrig, lineDir, bbP1, bbP2);
                if (p) {
                    // Ray intersects with current bb, update minimum distance
                    const auto distance = glm::distance(p.value(), lineOrig);
                    minDistanceY = std::min(minDistanceY, distance);
                }
            }
        }
        SDL_Log("Minimum dist %f. Number of bbs, %i", minDistanceY, m_outsideBBs.size());
        m_position.y = m_position.y + minDistanceY;
        Freeze();
        m_shouldLock = false;
    }

    void Shape::MarkToLock() {
        m_shouldLock = true;
    }

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
        const glm::mat4 rotate = glm::rotate(translate, m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        const glm::mat4 scale = glm::scale(rotate, glm::vec3(m_scale, 1.0f));
        return scale;
    }

    Shape::Shape(const float x, const float y,
                 const std::vector<glm::vec2> &tilePositions,
                 const std::vector<SDL_FRect> &outsideBBs) : m_position(glm::vec2(x, y)),
                                                             m_velocity(glm::vec2(0.0f)),
                                                             m_outsideBBs(outsideBBs) {
        // Initialize tiles
        m_tiles.reserve(tilePositions.size());
        const auto t = GetTransform();
        for (int i = 0; i < tilePositions.size(); i++) {
            m_tiles.emplace_back(std::make_shared<Tile>(tilePositions[i]));
            m_tiles[i]->parentTransform = t;
        }
    }
} // Tetris
