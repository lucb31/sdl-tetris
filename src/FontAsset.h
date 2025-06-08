//
// Created by lucas on 08.06.25.
//

#ifndef FONTASSET_H
#define FONTASSET_H
#include <string>
#include <map>

#include "Texture.h"


class FontAsset {
    struct Character {
        int w,h;
        float minU, maxU, minV, maxV;
        int offsetY;
        int xAdvance;
    };

public:
    FontAsset() = default;
    void Load(const std::string &filepath);

    Character GetCharacter(const char c);

    Texture* GetTexture() { return &m_texture; };

private:
    std::map<char, Character> m_characters;
    int m_fontSize{0}; int m_atlasWidth{0}; int m_atlasHeight{0};
    Texture m_texture;

    Character SampleCharacter(int charX, int charY, int charWidth, int charHeight, int offsetY, int xAdvance) const;
};


#endif //FONTASSET_H
