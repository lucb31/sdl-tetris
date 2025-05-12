#include <oneapi/tbb/profiling.h>
#include <SDL3/SDL.h>

#include "Renderer.h"

int main() {
    Renderer renderer;
    if (!renderer.init()) {
        SDL_Log("Could not initialize renderer");
        return 1;
    };
    SDL_Log("Init successful");

    // Main loop
    bool quit = false;

    SDL_Event event;
    SDL_zero(event);

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        renderer.render();
    }

    renderer.close();
    return 0;
}
