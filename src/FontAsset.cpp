#include "FontAsset.h"

#include <pugixml.hpp>

#include "SDL3/SDL_log.h"

void FontAsset::Load(const std::string &filepath) {
    // FIX: Pngs were not loaded correctly
    //m_texture.load("assets/test-font.png");

    // Load texture
    m_texture.load(filepath);
    m_texture.generate();
    m_fontSize = 24;

    // Load config
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("assets/256-font.xml");
    if (!result) {
        SDL_LogError(0, "Could not parse xml ");
        return;
    }
    m_atlasWidth = doc.child("font").child("common").attribute("scaleW").as_int();
    m_atlasHeight = doc.child("font").child("common").attribute("scaleH").as_int();

    for (pugi::xml_node tool: doc.child("font").child("chars").children("char")) {
        const auto id = tool.attribute("id").as_int();
        m_characters[static_cast<char>(id)] = SampleCharacter(
            tool.attribute("x").as_int(),
            tool.attribute("y").as_int(),
            tool.attribute("width").as_int(),
            tool.attribute("height").as_int(),
            tool.attribute("yoffset").as_int(),
            tool.attribute("xadvance").as_int()
            );
    }
}

FontAsset::Character FontAsset::SampleCharacter(int charX, int charY, int charWidth, int charHeight, int offsetY, int xAdvance) const {
    // Calc UV-Coordinates
    float y = float(m_atlasHeight - charY);
    float maxU = float(charX + charWidth) / m_atlasWidth;
    float minU = float(charX) / m_atlasWidth;
    float minV = y / m_atlasHeight;
    float maxV = (y - charHeight) / m_atlasHeight;

    return {
        charWidth, charHeight, minU, maxU, minV, maxV, offsetY, xAdvance
    };
}

FontAsset::Character FontAsset::GetCharacter(const char c) {
    // Will use default initializer and add to map if char unknown
    return m_characters[c];
}
