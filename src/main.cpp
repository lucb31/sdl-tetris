#include <oneapi/tbb/profiling.h>
#include <SDL3/SDL.h>

#include "Renderer.h"

constexpr int targetFps = 60;
constexpr Uint64 nsPerFrame = 1e9 / targetFps;

int main() {
    Renderer renderer;
    if (!renderer.init()) {
        SDL_Log("Could not initialize renderer");
        return 1;
    };
    SDL_Log("Init successful");

    // Main loop
    bool quit = false;
    int frames = 0;

    SDL_Event event;
    SDL_zero(event);

    // Debug FPS Calculation setup
    while (!quit) {
        const Uint64 frame_start_time = SDL_GetTicksNS();

        // Listen for events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Render current frame
        renderer.render();
        frames++;
        // Cap FPS: Delay render of next frame
        const Uint64 frame_render_time_ns = SDL_GetTicksNS() - frame_start_time;
        const Uint64 wait_time_ns = nsPerFrame - frame_render_time_ns;
        const auto wait_time_ms = static_cast
                <Uint32>(static_cast<double>(wait_time_ns) / 1e6);
        if (wait_time_ms > 0) {
            SDL_Delay(wait_time_ms);
        }
    }

    renderer.close();
    return 0;
}
