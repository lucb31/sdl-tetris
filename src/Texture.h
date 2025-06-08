//
// Created by lucas on 08.06.25.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture {
public:
    bool isReady() const {
        return m_width != 0 && m_height != 0;
    }

    // Load texture file
    void load(const std::string &filename);

    // Generates texture from image data
    void generate() const;

    // Binds the texture as the current active GL_TEXTURE_2D texture object
    void setActive() const;

    Texture();

    // Texture Format
    GLuint m_internalFormat{GL_RGB}; // Format of texture object
    GLuint m_imageFormat{GL_RGB}; // Format of loaded image

private:
    // Image dimensions
    int m_width{0}, m_height{0}, n{0};
    // OpenGL texture object
    GLuint m_id{0};

    // Temporary texture image data buffer
    unsigned char *m_imageData{nullptr};

    // Texture configuration
    GLint m_wrapS{GL_REPEAT}; // Wrapping mode on S axis
    GLint m_wrapT{GL_REPEAT}; // Wrapping mode on T axis
    GLfloat m_filterMin{GL_LINEAR}; // Filtering mode if texture pixels < screen pixels
    GLfloat m_filterMax{GL_LINEAR}; // Filtering mode if texture pixels > screen pixels
};


#endif //TEXTURE_H
