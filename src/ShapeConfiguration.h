//
// Created by lucas on 27.07.25.
//

#ifndef SHAPECONFIGURATION_H
#define SHAPECONFIGURATION_H
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

struct ShapeConfiguration {
    std::vector<glm::vec2> tilePositions;
    glm::vec4 color{};

    ShapeConfiguration() {
        tilePositions.reserve(4);
    }
};


#endif //SHAPECONFIGURATION_H
