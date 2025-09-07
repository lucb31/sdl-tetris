#include "Renderer.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <format>

bool Renderer::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize. Received error %s\n", SDL_GetError());
        return false;
    }

    // Setup OpenGL
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // Request OpenGL 3.3 core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    m_window = SDL_CreateWindow("Tetris", kScreenWidth, kScreenHeight, SDL_WINDOW_OPENGL);
    m_glContext = SDL_GL_CreateContext(m_window);

    // Init GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        SDL_LogError(0, "GLEW initialization error: %s", glewGetErrorString(glewError));
        SDL_GL_DestroyContext(m_glContext);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    SDL_LogInfo(0, "Initialized Renderer '%s' with OpenGL Version '%s'", renderer, version);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Place window on second monitor
    if (!SDL_SetWindowPosition(m_window, -3000, 200)) {
        SDL_Log("SDL could reposition window. Received error %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void Renderer::CheckShaderCompilation(GLuint shader, const char *type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        SDL_LogError(0, "Shader compilation error: %s (Type %s)", infoLog, type);
    }
}

void Renderer::CheckShaderLinking(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        SDL_LogError(0, "Shader linking error: %s", infoLog);
    }
}

static std::string GetShaderPath(const std::string& path) {
    return std::format("{}/{}", SHADERS_DIR, path);
}

/**
 *
 * @param vertexInputPath
 * @param fragmentInputPath
 * @return Shader program
 */
GLuint Renderer::LoadShader(const char *vertexInputPath, const char *fragmentInputPath) {
    // Open file streams for vertex & frag shader
    std::string vertexPath = GetShaderPath(vertexInputPath);
    std::string fragmentPath = GetShaderPath(fragmentInputPath);
    std::ifstream vShaderFile(vertexPath.c_str());
    if (!vShaderFile.is_open()) {
        SDL_LogError(0, "Could not open vertex shader at '%s'", vertexPath.c_str());
        return 0;
    }
    std::ifstream fShaderFile(fragmentPath.c_str());
    if (!fShaderFile.is_open()) {
        SDL_LogError(0, "Could not open fragment shader at '%s'", fragmentPath.c_str());
        return 0;
    }
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    std::string vShaderCode = vShaderStream.str();
    std::string fShaderCode = fShaderStream.str();

    const char *vShaderSource = vShaderCode.c_str();
    const char *fShaderSource = fShaderCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderSource, NULL);
    glCompileShader(vertexShader);
    CheckShaderCompilation(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
    glCompileShader(fragmentShader);
    CheckShaderCompilation(fragmentShader, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    CheckShaderLinking(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Cleanup: Close file streams
    vShaderFile.close();
    fShaderFile.close();
    return shaderProgram;
}


void Renderer::CheckGLError(const char *function) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_LogError(0, "OpenGL Error in %s: %s", function, gluErrorString(err));
    }
}

void Renderer::close() {
    SDL_GL_DestroyContext(m_glContext);
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    SDL_Quit();
}

std::array<float, 8> Renderer::RectToVec2(const SDL_FRect &rect) {
    return {
        rect.x, rect.y,
        rect.x + rect.w, rect.y,
        rect.x + rect.w, rect.y + rect.h,
        rect.x, rect.y + rect.h
    };
}

void Renderer::DrawSDLFRectsOutline(const SDL_FRect *rects, int count) {
    // TODO: PARAMETER
    constexpr std::array<float, 4> color = {1, 0, 1, 1};
    // Calc positions & indices
    std::vector<float> positions;
    // 4 vertices with 2 dimensions
    positions.reserve(count * 4 * 2);
    for (int i = 0; i < count; i++) {
        const auto arrayPositions = RectToVec2(rects[i]);
        for (const float &pos: arrayPositions) {
            positions.push_back(pos);
        }
    }
    const float mvp[] = {
        2.0f / kScreenWidth, 0, 0, 0,
        0, -2.0f / kScreenHeight, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1,
    };

    // Setup buffers
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    // Load shaders
    GLuint shaderProgram = LoadShader("rect.vert", "rect.frag");
    // Bind vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);

    // Bind position attribute
    const auto posLocation = glGetAttribLocation(shaderProgram, "pos");
    // Size 2 -> 2 vector dimensions
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posLocation);

    // Render
    glUseProgram(shaderProgram);
    CheckGLError("glUseProgram");
    // Bind mvp
    const auto mvpLocation = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLocation, 1, false, mvp);
    CheckGLError("glUniformMatrix4fv");
    // Bind color
    const auto colorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform4fv(colorLocation, 1, color.data());
    CheckGLError("glUniform4fv");
    // Bind vertex attributes & draw
    glBindVertexArray(VAO);
    CheckGLError("glBindVertexArray");
    for (int i = 0; i < count; i++) {
        glDrawArrays(GL_LINE_LOOP, i*4, 4);
    }
    CheckGLError("glDrawElements");
    glBindVertexArray(0);
}

void Renderer::DrawSDLRects(const SDL_FRect *rects, const int count, const std::array<float, 4> color) {
    // Calc positions & indices
    std::vector<float> positions;
    // 4 vertices with 2 dimensions
    positions.reserve(count * 4 * 2);
    // 2 triangles each connecting 3 vertices
    const unsigned int templateIndices[] = {0, 1, 2, 0, 2, 3};
    std::vector<unsigned int> indices;
    indices.reserve(count * 6);
    for (int i = 0; i < count; i++) {
        const auto arrayPositions = RectToVec2(rects[i]);
        for (const float &pos: arrayPositions) {
            positions.push_back(pos);
        }
        for (const unsigned int templateIndice: templateIndices) {
            indices.push_back(i * 4 + templateIndice);
        }
    }
    const float mvp[] = {
        2.0f / kScreenWidth, 0, 0, 0,
        0, -2.0f / kScreenHeight, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1,
    };

    // Setup buffers
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    // Load shaders
    GLuint shaderProgram = LoadShader("rect.vert", "rect.frag");
    // Bind vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
    // Bind index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

    // Bind position attribute
    const auto posLocation = glGetAttribLocation(shaderProgram, "pos");
    // Size 2 -> 2 vector dimensions
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(posLocation);

    // Render
    glUseProgram(shaderProgram);
    CheckGLError("glUseProgram");
    // Bind mvp
    const auto mvpLocation = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLocation, 1, false, mvp);
    CheckGLError("glUniformMatrix4fv");
    // Bind color
    const auto colorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform4fv(colorLocation, 1, color.data());
    CheckGLError("glUniform4fv");
    // Bind vertex attributes & draw
    glBindVertexArray(VAO);
    CheckGLError("glBindVertexArray");
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    CheckGLError("glDrawElements");
    glBindVertexArray(0);
}

void Renderer::DrawSDLRects(const SDL_FRect *rects, int count) {
    constexpr std::array<float, 4> defaultColor = {1, 0, 1, 1};
    return DrawSDLRects(rects, count, defaultColor);
}

void Renderer::BeginRender() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::EndRender() {
    SDL_GL_SwapWindow(m_window);
}
