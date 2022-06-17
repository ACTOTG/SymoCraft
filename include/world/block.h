#pragma once
#include <yaml-cpp/yaml.h>
#include "core.h"

namespace SymoCraft {
    constexpr uint16 NULL_BLOCK_ID = 0;

    struct BlockFormat {
        uint16 m_top_texture;
        uint16 m_side_texture;
        uint16 m_bottom_texture;
        bool m_is_transparent;
        bool m_is_solid;
        bool m_is_blendable;
        bool m_is_lightSource;
        int16 light_level;
    };

    void loadBlocks(std::string_view block_format_config);
    uint16 get_block_id(const std::string& block_name);
    const BlockFormat& get_block(int block_id);
    const BlockFormat& get_block(const std::string& name);

    class Block {
    public:
        // Gabe's reference note
        // 64 bits per block
        // 16 bits integer id 2^16
        //  4 bits light level 0-15
        //  4 bits rotation direction
        //  8 bits block type
        // 32 bits extra stuff
        uint16 block_id;
        int16 lightLevel;
        int16 lightColor;

        // Bit 1 IsTransparent
        // Bit 2 isBlendable
        // Bit 3 is_lightSource
        uint16 bitwise_compressed_data;

        inline bool operator==(const Block& b) const
        {
            return block_id == b.block_id;
        }

        inline bool operator!=(const Block& b) const
        {
            return block_id != b.block_id;
        }

        inline bool IsTransparent() const
        {
            return (bitwise_compressed_data & (1 << 0));
        }

        inline bool isBlendable() const
        {
            return (bitwise_compressed_data & (1 << 1));
        }

        inline bool isLightSource() const
        {
            return (bitwise_compressed_data & (1 << 2));
        }

        inline bool isLightPassable() const
        {
            return isLightSource() || IsTransparent();
        }

        inline void setTransparent(bool transparent)
        {
            // Clear the bit
            bitwise_compressed_data &= ~(1 << 0);
            // Set the bit if needed
            bitwise_compressed_data |= transparent ? (1 << 0) : 0;
        }

        inline void setIsBlendable(bool isBlendable)
        {
            // Clear the bit
            bitwise_compressed_data &= ~(1 << 1);
            // Set the bit if needed
            bitwise_compressed_data |= isBlendable ? (1 << 1) : 0;
        }

        inline void setIsLightSource(bool isLightSource)
        {
            // Clear the bit
            bitwise_compressed_data &= ~(1 << 2);
            // Set the bit if needed
            bitwise_compressed_data |= isLightSource ? (1 << 2) : 0;
        }

        inline void setLightColor(const glm::ivec3& color)
        {
            // Convert from number between 0-255 to number between 0-7
            lightColor =
                    (( ((int)((float)color.r / 255.0f) * 7) << 0) & 0x7)  |
                    (( ((int)((float)color.g / 255.0f) * 7) << 3) & 0x38) |
                    (( ((int)((float)color.b / 255.0f) * 7) << 6) & 0x1C0);
        }

        inline glm::ivec3 getLightColor() const
        {
            // Convert from number between 0-7 to number between 0-255
            return {
                    (int)(((float)((lightColor & 0x7)   >> 0) / 7.0f) * 255.0f),  // R
                    (int)(((float)((lightColor & 0x38)  >> 3) / 7.0f) * 255.0f),  // G
                    (int)(((float)((lightColor & 0x1C0) >> 6) / 7.0f) * 255.0f)   // B
            };
        }

        inline glm::ivec3 getCompressedLightColor() const
        {
            return {((lightColor & 0x7) >> 0),  // R
                    ((lightColor & 0x38) >> 3), // G
                    ((lightColor & 0x1C0) >> 6) // B
            };
        }

        inline bool IsNullBlock() const
        {
            return block_id == NULL_BLOCK_ID;
        }

    };

    extern Block NULL_BLOCK;
    extern Block AIR_BLOCK;

}
