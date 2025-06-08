//
// Created by lucas on 08.06.25.
//

#ifndef SHADER_H
#define SHADER_H

#include "Renderer.h"
#include <string>

class Shader {
public:
    Shader() = default;

    void Load(const std::string &vertexPath, const std::string &fragmentPath);
    void Use() const;

    static int LoadShader(const std::string &vertexPath, const std::string &fragmentPath);

    static void CheckShaderCompilation(GLuint shader, const char *type);

    static void CheckShaderLinking(GLuint program);

    explicit operator GLuint() const { return m_programId; }

private:
    GLuint m_programId{0};
};



#endif //SHADER_H
