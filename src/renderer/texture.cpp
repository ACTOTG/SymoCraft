#include "renderer/texture.h"

Texture Texture::CreateRegularTexture(const std::string_view &filepath, bool pixelated)
{
    stbi_set_flip_vertically_on_load(true);

    Texture res;
    res.m_filepath = filepath;
    uint8* pixels = stbi_load(res.m_filepath.c_str(), &res.m_width, &res.m_height, &res.m_channel_amount, 0);

    res.m_texture_format = GL_RGBA;
    res.m_internal_format = GL_RGBA32F;
    if (res.m_channel_amount == 3)
    {
        res.m_texture_format = GL_RGB;
        res.m_internal_format = GL_RGB32F;
    }
    else if (res.m_channel_amount != 4)
    {
        std::cerr << "Unknown number of channels " << res.m_channel_amount << "in image " << res.m_channel_amount, res.m_filepath.c_str();
        stbi_image_free(pixels);
        return Texture{};
    }

    // Generate and bind the texture object
    glCreateTextures(GL_TEXTURE_2D, 1, &res.m_texture_Id);

    glTextureParameteri(res.m_texture_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(res.m_texture_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(res.m_texture_Id, GL_TEXTURE_MIN_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);
    glTextureParameteri(res.m_texture_Id, GL_TEXTURE_MAG_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);

    if (pixels)
    {
        glTextureStorage2D(res.m_texture_Id, 1,
                           res.m_internal_format,
                           res.m_width, res.m_height);
        glTextureSubImage2D(res.m_texture_Id, 0,
                            0, 0,
                            res.m_width, res.m_height,
                            res.m_texture_format, GL_UNSIGNED_BYTE, pixels);
        glGenerateTextureMipmap(res.m_texture_Id);
    }
    else
    {
        printf("STB failed to load image: %s\n-> STB Failure Reason: %s", res.m_filepath.c_str(), stbi_failure_reason() );
    }

    stbi_image_free(pixels);

    return res;
}

TextureArray TextureArray::CreateAtlasSlice(const std::string &filepath, bool pixelated)
{
    // Set up
    Texture tex_atlas = CreateRegularTexture( filepath, pixelated );

    constexpr GLsizei tile_width = 16, tile_height = 16;
    GLsizei tile_columns = tex_atlas.m_width / tile_width,
            tile_rows = tex_atlas.m_height / tile_height,
            tile_quantity = tile_columns * tile_rows;


    // Create texture array to store slices
    TextureArray tile_set;
    tile_set.layer_amount = tile_quantity;
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tile_set.m_texture_Id);
    glTextureStorage3D(tile_set.m_texture_Id, 1, GL_RGBA32F, tile_width, tile_height, tile_quantity);

    {
        for (GLsizei i = 0; i < tile_quantity; ++i)
        {
            // For "glCopyImageSubData", (0, 0, 0) is at the bottom left
            GLint x_coord = (i % tile_columns) * tile_width, // Whenever (i % tile_columns) == 0, reset to the first column
                  y_coord = (i / tile_columns) * tile_height;  // Whenever (i / tile_columns)++, switch to the next row(upward)
            glCopyImageSubData(tex_atlas.m_texture_Id, GL_TEXTURE_2D,
                               0, x_coord, y_coord, 0,  // Source texture atlas mipmap level, x coord, y coord, z coord(null)
                               tile_set.m_texture_Id, GL_TEXTURE_2D_ARRAY,
                               0, 0, 0, i,  // Destiny texture array mipmap level, x coord, y coord, z coord(layers)
                               tile_width, tile_height, 1);
        }
        glDeleteTextures(1, &tex_atlas.m_texture_Id);
    }

    glTextureParameteri(tile_set.m_texture_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tile_set.m_texture_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(tile_set.m_texture_Id, GL_TEXTURE_MIN_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);
    glTextureParameteri(tile_set.m_texture_Id, GL_TEXTURE_MAG_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);

    glGenerateTextureMipmap(tile_set.m_texture_Id);

    return tile_set;
}
