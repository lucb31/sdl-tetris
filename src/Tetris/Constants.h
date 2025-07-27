//
// Created by lucas on 25.05.25.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Tetris {
    constexpr int widthPerTile = 32;
    constexpr int heightPerTile = 32;
    constexpr int tileRows = 20;
    constexpr int tileCols = 10;
    constexpr int totalNrOfTiles = tileRows * tileCols;
    constexpr int boardSizeX = tileCols * widthPerTile;
    constexpr int boardSizeY = tileRows * heightPerTile;
}

#endif //CONSTANTS_H
