//
// Created by lucas on 27.07.25.
//

#ifndef SHAPECONFIGURATION_H
#define SHAPECONFIGURATION_H
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec4.hpp"

namespace Tetris {
    struct ShapeConfiguration {
        // Used mainly for debugging purposes
        std::string name;
        std::vector<glm::vec2> tilePositions;
        glm::vec4 color{};

        ShapeConfiguration() {
            tilePositions.reserve(4);
            name = "Unnamed Shape";
        }
    };

}

#endif //SHAPECONFIGURATION_H
