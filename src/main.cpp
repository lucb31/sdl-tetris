#include <SDL3/SDL.h>

#include "Tetris/Board.h"
#include "Renderer.h"
#include "Benchmark/Instrumentor.h"

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

    // Initialize game
    Tetris::Board board;

    // Main loop
    bool quit = false;
    int frames = 0;

    SDL_Event event;
    SDL_zero(event);

    Benchmark::Instrumentor::Instance().beginSession("Tetris");
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
                if (event.key.key == SDLK_ESCAPE) {
                    quit = true;
                }
            }
        }

        // Advance frame
        board.Tick(dt);

        renderer.BeginRender();
        board.Draw();
        renderer.EndRender();
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

    Benchmark::Instrumentor::Instance().endSession();
    renderer.close();
    return 0;
}
