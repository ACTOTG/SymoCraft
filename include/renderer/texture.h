#ifndef TEXTURE_H
#define TEXTURE_H
#include <stb/stb_image.h>
#include "core.h"

using layer_t = int;

class Texture
{
public:
    Texture CreateRegularTexture(const std::string_view &filepath, bool pixelated);

    std::string m_filepath{};
    int m_width{};
    int m_height{};
    int m_channel_amount{};
    GLint m_texture_format{};
    GLint m_internal_format{};
    uint32 m_texture_Id{};
};

class TextureArray : public Texture
{
public:
    TextureArray CreateAtlasSlice(const std::string &filepath, bool pixelated);

    int layer_amount{};
};

#endif