//
// Created by lucas on 27.07.25.
//

#ifndef COLLISION_H
#define COLLISION_H
#include <optional>

#include "glm/geometric.hpp"
#include "glm/vec2.hpp"
#include "SDL3/SDL_rect.h"

struct Collision {
    SDL_FRect a;
    SDL_FRect b;
    SDL_FRect intersection;
};

// Function to check ray and line segment intersection
inline std::optional<glm::vec2> RayIntersectsSegmentAt(
    const glm::vec2 &rayOrigin,
    const glm::vec2 &rayDir,
    const glm::vec2 &segA,
    const glm::vec2 &segB,
    const float epsilon = 1e-6f
) {
    glm::vec2 u = rayDir; // Direction of the ray
    glm::vec2 v = segB - segA; // Direction of the segment
    glm::vec2 w = rayOrigin - segA;

    float a = glm::dot(u, u); // always >= 0
    float b = glm::dot(u, v);
    float c = glm::dot(v, v); // always >= 0
    float d = glm::dot(u, w);
    float e = glm::dot(v, w);
    float D = a * c - b * b; // always >= 0

    if (std::abs(D) < epsilon) {
        // Parallel case
        return std::nullopt;
    }

    float sc = (b * e - c * d) / D;
    float tc = (a * e - b * d) / D;

    // Check if intersection lies on the ray (sc >= 0) and within the segment (tc ∈ [0,1])
    if (sc < 0.0f || tc < 0.0f || tc > 1.0f) {
        return std::nullopt;
    }

    glm::vec2 intersection = rayOrigin + sc * u;
    return intersection;
}


#endif //COLLISION_H
