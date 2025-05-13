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
    constexpr int sample_size = 60;
    Uint64 fps_calc_start_time = SDL_GetTicks();
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
        const Uint64 frame_render_time_ns = SDL_GetTicksNS() - frame_start_time;

        // DEBUG: Log avg framerate
        if (frames % sample_size == 0) {
            const Uint64 end = SDL_GetTicks();
            // time passed in milliseconds for 10000 frames
            const Uint64 diff = end - fps_calc_start_time;

            // *1000 to convert from frames per ms to frames per s
            const float avg_fps = static_cast<double>(sample_size) / static_cast<double>(diff) * 1000.0f;
            SDL_Log("FPS: %.1f", avg_fps);

            // Update reference point
            fps_calc_start_time = end;
        }

        // Cap FPS: Delay render of next frame
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
