//
// Created by lucas on 25.05.25.
//

#include "Tile.h"

#include "Constants.h"

namespace Tetris {
    SDL_FRect Tile::BB() const {
        // Translate to world position
        Math::Vec3 globalPos = parentTransform * Math::Vec3(m_position.x(), m_position.y(), 1);
        return SDL_FRect{globalPos.x(), globalPos.y(), widthPerTile, heightPerTile };
    }

    void Tile::Draw(SDL_Renderer* renderer) {
        SDL_FRect bb = BB();
        // TODO: Enums for colors
        // Fill
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderFillRect(renderer, &bb);
        // Outline
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderRect(renderer, &bb);
    }
} // Tetris