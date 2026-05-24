#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

struct TTFGlyph
{
    Texture2D* texture;
    glm::ivec2 size;
    glm::ivec2 bearing;
    int advance;
};
