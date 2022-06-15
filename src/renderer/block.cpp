#include "renderer/block.h"

void Block::loadBlocks(std::string_view block_format_config){
    YAML::Node block_format = YAML::LoadFile(block_format_config.data());

    for (auto block : block_format)
    {
        if(!block.second["id"].IsDefined())
        {
            std::cerr << "All blocks must have a block id defined. Block"
                      << block.first.as<std::string>() << "does not have an id.";
        }
    }
}