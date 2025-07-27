//
// Created by lucas on 25.05.25.
//

#include "Tile.h"

#include "Board.h"
#include "Constants.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Tetris {
    SDL_FRect Tile::BB() const {
        const glm::vec3 globalPos = GetGlobalTopLeft();
        const float discreteX = std::round(globalPos.x / widthPerTile) * widthPerTile;
        const float discreteY = std::round(globalPos.y / heightPerTile) * heightPerTile;
        return SDL_FRect{discreteX, discreteY, widthPerTile, heightPerTile};
    }

    glm::vec3 Tile::GetGlobalTopLeft() const {
        // Apply parent transform to local position to retrieve global center position
        const glm::vec4 globalPos = parentTransform * glm::vec4(position.x, position.y, 1, 1);
        // Offset from center position to top left
        constexpr glm::mat4 localTransform = glm::translate(glm::mat4(1.0f),
                                                            glm::vec3(-widthPerTile / 2.0f, -heightPerTile / 2.0f, 0));
        return localTransform * globalPos;
    }
} // Tetris
