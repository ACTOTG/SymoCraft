#pragma once

#include <yaml-cpp/yaml.h>
#include "core.h"

class Block
{
public:
    void loadBlocks(std::string_view block_format_config);

    uint16 id;

    struct BlockFormat
    {
        uint16 top_texture;
        uint16 side_texture;
        uint16 bottom_texture;
        bool is_solid;
        bool is_transparent;
    };

};