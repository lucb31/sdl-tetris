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


class StringRenderer {
public:
    StringRenderer();

    void Render();

    void SetString(const std::string &str);

    std::vector<float> SampleCharacters(const std::string &str);

    std::array<float, 16> proj{};

private:
    FontAsset m_font{};
    Shader m_shader;
    GLuint m_vao{}, m_vbo{}, m_ebo{};

    // Number of characters the string buffer can hold
    unsigned int m_stringBufferCapacity{16};
    // Number of characters currently stored
    unsigned int m_stringBufferSize{0};
};


#endif //STRINGRENDERER_H
