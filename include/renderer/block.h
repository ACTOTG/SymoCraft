#pragma once
#include <yaml-cpp/yaml.h>
#include "core.h"

namespace SymoCraft {
    constexpr uint16 NULL_BLOCK_ID = 0;

    // Gabe's reference note
    // 64 bits per block
    // 16 bits integer id 2^16
    //  4 bits light level 0-15
    //  4 bits rotation direction
    //  8 bits block type
    // 32 bits extra stuff

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

    class Block {
    public:
        void loadBlocks(std::string_view block_format_config);

        uint16 block_id;
        int16 lightLevel;
        int16 lightColor;

        // Bit 1 isTransparent
        // Bit 2 isBlendable
        // Bit 3 is_lightSource
        uint16 bitwise_compressed_data;


        inline bool is_null_block() const
        {
            return block_id == NULL_BLOCK_ID;
        }

        uint16 get_block_id(const std::string& block_name);
        const BlockFormat& get_block(int block_id);
        const BlockFormat& get_block(const std::string& name);

    private:

    };

    inline bool operator==(const Block& a, const Block& b)
    {
        return a.block_id == b.block_id;
    }

    inline bool operator!=(const Block& a, const Block& b)
    {
        return a.block_id != b.block_id;
    }

    extern Block NULL_BLOCK;
    extern Block AIR_BLOCK;

}
