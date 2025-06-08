//
// Created by lucas on 08.06.25.
//

#ifndef STRINGRENDERER_H
#define STRINGRENDERER_H
#include <array>
#include <string>
#include <vector>

#include "FontAsset.h"
#include "Shader.h"
#include "glm/glm.hpp"


class StringRenderer {
public:
    StringRenderer() : StringRenderer(16) {}
    explicit StringRenderer(const int capacity);

    void Render();

    void SetString(const std::string &str);

    std::vector<float> SampleCharacters(const std::string &str);

    std::array<float, 16> proj{};

    void SetPosition(glm::vec2 pos);

    void UpdateTransform();

private:
    // Transform
    glm::mat4 m_transform;
    glm::vec2 m_position{};

    // Rendering
    FontAsset m_font{};
    Shader m_shader;
    GLuint m_vao{}, m_vbo{}, m_ebo{};

    // Number of characters the string buffer can hold
    unsigned int m_stringBufferCapacity;
    // Number of characters currently stored
    unsigned int m_stringBufferSize{0};
};


#endif //STRINGRENDERER_H
