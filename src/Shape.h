//
// Created by lucas on 23.05.25.
//

#ifndef SHAPE_H
#define SHAPE_H
#include <memory>
#include <vector>

#include "Collision.h"
#include "GameObject.h"
#include "ShapeConfiguration.h"
#include "Tile.h"

namespace Tetris {
    constexpr glm::vec2 GRAVITY_VECTOR = glm::vec2(0.0f, 150.0f);

    class Shape : public GameObject {
        // Controls how 'maneuverable' the shape is
        float m_speed{150};
        bool m_grounded{false};

        // Tiles need to be shared ptrs since they're going to get
        // passed to the board once shape is placed
        std::vector<std::shared_ptr<Tile> > m_tiles;
        // Current position
        glm::vec2 m_position{0.0f};
        // Current rotation in RAD
        float m_rotation{0.0f};
        // Current velocity
        glm::vec2 m_velocity{0.0f};
        // Requested velocity by user input
        glm::vec2 m_inputVelocity{0.0f};

        // Bounding boxes the shape could collide with
        // Will be initialized with shape
        std::vector<SDL_FRect> m_outsideBBs;

        glm::vec4 m_color{1.0f};

        void CalculateVelocity();

        std::vector<Collision> CalculateCollisions() const;

    public:
        Shape(float x, float y, const std::vector<glm::vec2> &tilePositions, const std::vector<SDL_FRect> &outsideBBs);

        Shape(float x, float y, const ShapeConfiguration &config, const std::vector<SDL_FRect> &outsideBBs) : Shape(
            x, y, config.tilePositions, outsideBBs) {
            m_color = config.color;
        };

        Shape(const glm::vec2 &pos, const ShapeConfiguration &config,
              const std::vector<SDL_FRect> &outsideBBs) : Shape(pos.x, pos.y, config, outsideBBs) {
        };

        bool IsGrounded() const { return m_grounded; }

        std::vector<std::shared_ptr<Tile> > GetTiles() const { return m_tiles; }

        void Freeze();

        void AddInputVelocity(const glm::vec2 &direction);

        void Rotate(float radians);

        void Draw() override;

        void Tick(float dt) override;

        void AttemptRotation(const float &rotation);

        void UpdateTransform() const;

        void MoveAndSlide(float dt);

        std::vector<SDL_FRect> GetCollisionBBs() const;

        glm::mat4 GetTransform() const;
    };
} // Tetris

#endif //SHAPE_H
