//
// Created by lucas on 12.05.25.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <array>

#include "Vec3.h"

constexpr int kScreenWidth = 1024;
constexpr int kScreenHeight = 768;

class Renderer {
public:
    bool init();

    static GLuint LoadShader(const char *vertexPath, const char *fragmentPath);

    static void CheckGLError(const char *function);

    void BeginRender();

    void EndRender();

    void close();

    static std::array<float, 8> RectToVec2(const SDL_FRect &rect);

    static void DrawSDLRects(const SDL_FRect *rects, int count);
    static void DrawSDLRects(const SDL_FRect *rects, int count, std::array<float, 4> color);

private:
    // TODO: Deprecate
    SDL_Renderer *m_renderer{nullptr};
    SDL_GLContext m_glContext{nullptr};
    SDL_Window *m_window{nullptr};

    static void CheckShaderCompilation(GLuint shader, const char *type);

    static void CheckShaderLinking(GLuint program);
};

#endif //RENDERER_H