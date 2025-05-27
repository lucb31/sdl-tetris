//
// Created by lucas on 25.05.25.
//

#include "Tile.h"

#include "Constants.h"

namespace Tetris {
    SDL_FRect Tile::BB() const {
        const Math::Vec3 globalPos = GetGlobalTopLeft();
        const float discreteX = std::round(globalPos.x()  / widthPerTile) * widthPerTile;
        const float discreteY = std::round(globalPos.y()  / heightPerTile) * heightPerTile;
        return SDL_FRect{discreteX, discreteY, widthPerTile, heightPerTile};
    }

    Math::Vec3 Tile::GetGlobalTopLeft() const {
        // Apply parent transform to local position to retrieve global center position
        Math::Vec3 globalPos = parentTransform * Math::Vec3(m_position.x(), m_position.y(), 1);
        // Offset from center position to top left
        Math::Mat3 localTransform = Math::Mat3(
            1, 0, 0,
            0, 1, 0,
            -widthPerTile / 2, -heightPerTile / 2, 1);
        return localTransform * globalPos;
    }

    // Render floating point bb as outline
    void Tile::DrawRealPosition(SDL_Renderer *renderer) const {
        const Math::Vec3 globalPos = GetGlobalTopLeft();
        const auto floatingBB = SDL_FRect{globalPos.x(), globalPos.y(), widthPerTile, heightPerTile};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderRect(renderer, &floatingBB);
    }

    // Render discrete bb with fill color
    void Tile::DrawDiscretePosition(SDL_Renderer *renderer) const {
        // Render discrete BB
        SDL_FRect bb = BB();
        // TODO: Enums for colors
        // Fill
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderFillRect(renderer, &bb);
    }

    void Tile::Draw(SDL_Renderer *renderer) {
        DrawRealPosition(renderer);
        DrawDiscretePosition(renderer);
    }
} // Tetris
