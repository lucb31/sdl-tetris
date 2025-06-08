#include "Shader.h"

#include <fstream>
#include <sstream>

#include "Renderer.h"
#include <SDL3/SDL.h>

void Shader::Load(const std::string &vertexPath, const std::string &fragmentPath) {
    m_programId = LoadShader(vertexPath, fragmentPath);
}

void Shader::Use() const {
    glUseProgram(m_programId);
    Renderer::CheckGLError("glUseProgram");
}

int Shader::LoadShader(const std::string &vertexPath, const std::string &fragmentPath) {
    // Open file streams for vertex & frag shader
    std::ifstream vShaderFile(vertexPath);
    if (!vShaderFile.is_open()) {
        SDL_LogError(0, "Could not open vertex shader at '%s'", vertexPath);
        return 0;
    }
    std::ifstream fShaderFile(fragmentPath);
    if (!fShaderFile.is_open()) {
        SDL_LogError(0, "Could not open fragment shader at '%s/%s'", fragmentPath);
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

void Shader::CheckShaderCompilation(GLuint shader, const char *type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        SDL_LogError(0, "Shader compilation error: %s (Type %s)", infoLog, type);
    }
}

void Shader::CheckShaderLinking(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        SDL_LogError(0, "Shader linking error: %s", infoLog);
    }
}
