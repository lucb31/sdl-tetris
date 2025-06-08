//
// Created by lucas on 08.06.25.
//

#include "StringRenderer.h"
#include <SDL3/SDL.h>

StringRenderer::StringRenderer() {
    // Load font
    m_font = FontAsset();
    // TODO: Load png instead. JPG Dosent have opacity
    m_font.Load("assets/256-font.jpg");

    // Load shader
    m_shader.Load("src/Shaders/font.vert", "src/Shaders/font.frag");

    // Generate buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glBindVertexArray(m_vao);

    // Allocate memory for VBO buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    constexpr int vertexAttributes = 4;
    constexpr int verticesPerCharacter = 4;
    const int bufferSize = sizeof(float) * m_stringBufferCapacity * vertexAttributes * verticesPerCharacter;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
    Renderer::CheckGLError("glBufferData");

    // Fill index buffer
    std::vector<unsigned int> indices;
    indices.reserve(m_stringBufferCapacity * 6);
    const unsigned int templateIndices[] = {0, 1, 2, 0, 2, 3};
    for (int i = 0; i < m_stringBufferCapacity; i++) {
        for (const unsigned int templateIndice: templateIndices) {
            indices.push_back(i * 4 + templateIndice);
        }
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Bind position & uv attribute
    const auto posLocation = glGetAttribLocation((GLuint)m_shader, "pos");
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, vertexAttributes * sizeof(float), nullptr);
    glEnableVertexAttribArray(posLocation);
    const auto uvLocation = glGetAttribLocation((GLuint)m_shader, "uv");
    glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, vertexAttributes * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(uvLocation);
}

void StringRenderer::Render() {
    // Bind shader
    m_shader.Use();

    // Bind mvp uniform
    const auto mvpLocation = glGetUniformLocation(static_cast<GLuint>(m_shader), "mvp");
    glUniformMatrix4fv(mvpLocation, 1, false, proj.data());
    Renderer::CheckGLError("glUniformMatrix4fv");

    // Bind vertex attributes
    glBindVertexArray(m_vao);
    Renderer::CheckGLError("glBindVertexArray");

    // Bind font texture
    m_font.GetTexture()->setActive();

    // Draw
    glDrawElements(GL_TRIANGLES, 6*m_stringBufferSize, GL_UNSIGNED_INT, 0);
    Renderer::CheckGLError("glDrawElements");
    glBindVertexArray(0);
}

void StringRenderer::SetString(const std::string &str) {
    // Fill VBO buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    std::vector<float> vertices = SampleCharacters(str);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), vertices.data());
    Renderer::CheckGLError("glBufferSubData");

    m_stringBufferSize = str.size();
}

std::vector<float> StringRenderer::SampleCharacters(const std::string &str) {
    std::vector<float> vertices;
    vertices.reserve(str.size() * 4 * 4);

    float startX = 900;
    float startY = 200;
    for (const char &c: str) {
        const auto character = m_font.GetCharacter(c);

        const float endX = startX + character.w;
        const float endY = startY + character.h;

        // TODO: Consider line height & offset
        std::vector<float> characterVertices = {
            startX, startY, character.minU, character.minV,
            endX, startY, character.maxU, character.minV,
            endX, endY, character.maxU, character.maxV,
            startX, endY, character.minU, character.maxV,
        };
        vertices.insert(vertices.end(), characterVertices.begin(), characterVertices.end());
        startX = endX;
    }
    return vertices;
}
