#include "FontAsset.h"

void FontAsset::Load(const std::string &filepath) {
    // FIX: Pngs were not loaded correctly
    //m_texture.load("assets/test-font.png");

    // Load texture
    m_texture.load(filepath);
    m_texture.generate();
    m_fontSize = 24;

    // TODO: Parse configuration file instead
    m_characters['a'] = SampleCharacter(206, 68, 13, 15);
    m_characters['b'] = SampleCharacter(208, 0, 14, 20);
    m_characters['$'] = SampleCharacter(109, 0, 14, 22);
}

FontAsset::Character FontAsset::SampleCharacter(int charX, int charY, int charWidth, int charHeight) {
    // TODO: Needs to be stored on tex level
    const float texWidth = 256;
    const float texHeight = 256;

    // Calc UV-Coordinates
    float y = texHeight - charY;
    float maxU = (charX + charWidth) / texWidth;
    float minU = charX / texWidth;
    float minV = y / texHeight;
    float maxV = (y - charHeight) / texHeight;

    return {
        charWidth, charHeight, minU, maxU, minV, maxV
    };
}

FontAsset::Character FontAsset::GetCharacter(const char c) {
    // Will use default initializer and add to map if char unknown
    return m_characters[c];
}

