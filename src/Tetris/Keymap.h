//
// Created by lucas on 25.05.25.
//

#ifndef KEYMAP_H
#define KEYMAP_H

namespace Tetris {
    enum InputKey {
        MoveLeft = SDLK_A,
        MoveRight = SDLK_D,
        MoveUp = SDLK_W,
        MoveDown = SDLK_S,
        RotateRight = SDLK_E,
        RotateLeft = SDLK_Q,
        LockShape = SDLK_SPACE,
    };
}
#endif //KEYMAP_H
