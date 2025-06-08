//
// Created by lucas on 08.06.25.
//

#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Renderer.h"
#include "stb_image.h"
#include "SDL3/SDL_log.h"

void Texture::load(const std::string &filename) {
    stbi_set_flip_vertically_on_load(true);
    m_imageData = stbi_load(filename.c_str(), &m_width, &m_height, &n, 0);
    if (!m_imageData) {
        SDL_LogError(0, "Could not load %s", filename.c_str());
    }
    if ((m_width & (m_width - 1)) != 0 || (m_height & (m_height - 1)) != 0) {
        SDL_LogWarn(0, "Texture %s is not power of 2 dimension", filename.c_str());
    }
}

void Texture::generate() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        m_internalFormat,
        m_width,
        m_height,
        0,
        m_imageFormat,
        GL_UNSIGNED_BYTE,
        m_imageData
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT); // GL_CLAMP_TO_EDGE
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterMin);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterMax);
    glBindTexture(GL_TEXTURE_2D, 0);
    delete m_imageData;
}

void Texture::setActive() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

Texture::Texture() {
    glGenTextures(1, &m_id);
}
