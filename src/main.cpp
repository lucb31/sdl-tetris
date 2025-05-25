#include <oneapi/tbb/profiling.h>
#include <SDL3/SDL.h>

#include "Board.h"
#include "Mat3.h"
#include "Renderer.h"

constexpr int targetFps = 60;
constexpr float dt = 1.0f / targetFps;
constexpr Uint64 nsPerFrame = 1e9 / targetFps;

int main() {
    Renderer renderer;
    if (!renderer.init()) {
        SDL_Log("Could not initialize renderer");
        return 1;
    };
    SDL_Log("Init successful");

    // Column major order
    // Test cases for matrix x matrix multiplication
    Math::Mat3 a{1, 9, 6, 8, 4, 2, 3, 5, 7};
    Math::Mat3 b{6, 1, 5, 7, 3, 9, 4, 2, 8};
    Math::Mat3 c = a * b;
    Math::Mat3 expectedC{29, 83, 73, 58, 120, 111, 44, 84, 84};
    if (!std::equal(std::begin(c.e), std::end(c.e), std::begin(expectedC.e))) {
        SDL_Log("Mat3 mismatch");
    }

    // Test cases for matrix x vector multiplication
    Math::Mat3 m{1,4,7,2,5,8,3,6,9};
    Math::Vec3 v{2,1,3};
    Math::Vec3 res = m*v;
    Math::Vec3 expectedRes{13, 31, 49};
    if (!std::equal(std::begin(res.e), std::end(res.e), std::begin(expectedRes.e))) {
        SDL_Log("Vec3 testcase failed");
    }

    // Initialize game
    Tetris::Board board;

    // Main loop
    bool quit = false;
    int frames = 0;

    SDL_Event event;
    SDL_zero(event);

    while (!quit) {
        const Uint64 frame_start_time = SDL_GetTicksNS();

        // Listen for events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                board.HandleKeyDown(event.key);
            } else if (event.type == SDL_EVENT_KEY_UP) {
                board.HandleKeyUp(event.key);
            }
        }

        // Advance frame
        board.Tick(dt);

        // Render current frame
        renderer.render(&board);
        frames++;

        // Cap FPS: Delay render of next frame
        const Uint64 frame_render_time_ns = SDL_GetTicksNS() - frame_start_time;
        if (frame_render_time_ns > nsPerFrame) {
            continue;
        }
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
