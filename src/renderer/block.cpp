#include "renderer/block.h"

namespace SymoCraft{
    static robin_hood::unordered_flat_map<uint16, BlockFormat> block_format_map;
    static robin_hood::unordered_flat_map<std::string, uint16> name_to_id_map;

    uint16 Block::get_block_id(const std::string& block_name)
    {
        const auto& iter = name_to_id_map.find(block_name);
        if (iter == name_to_id_map.end())
        {
            return 0;
        }
        return iter->second;
    }

    const BlockFormat& Block::get_block(int block_id)
    {
        if (block_format_map.contains(block_id))
        {
            return block_format_map[block_id];
        }
        return block_format_map[0];
    }


    const BlockFormat& Block::get_block(const std::string& name)
    {
        int blockId = get_block_id(name);
        return block_format_map[blockId];
    }

    void Block::loadBlocks(std::string_view block_format_config)
    {
        YAML::Node block_formats = YAML::LoadFile(block_format_config.data());

        block_format_map[0] = {1, 1, 1,
                               true, false, false,
                               false, 0};

        for (auto block : block_formats)
        {

            AmoLogger_Notice( !block.second["id"].IsDefined(), "All blocks must have a block id defined. Block '%s' does not have an id.", block.first.as<std::string>().c_str() );
            int id = block.second["id"].as<int>();
            name_to_id_map[block.first.as<std::string>()] = id;

            uint16 side_texture =  block.second["side"].as<uint16>();
            uint16 top_texture= block.second["top"].as<uint16>();
            uint16 bottom_texture = block.second["bottom"].as<uint16>();
            bool isTransparent = block.second["isTransparent"].as<bool>();
            bool isSolid = block.second["isSolid"].as<bool>();
            bool isBlendable = block.second["isBlendable"].IsDefined() && block.second["isBlendable"].as<bool>();
            bool isLightSource = block.second["is_lightSource"].IsDefined() && block.second["is_lightSource"].as<bool>();
            int16 lightLevel = block.second["light_level"].IsDefined() ? block.second["light_level"].as<int16>() : 0;

            if (block_format_map.contains(id))
            {
                AmoLogger_Warning("Block format detected a duplicate block id '%d'. Do you have two blocks with id '%d'?", id, id);
            }

            block_format_map[id] = BlockFormat{
                    side_texture, top_texture, bottom_texture,
                    isTransparent, isSolid, isBlendable,
                    isLightSource, lightLevel };

        }
    }
}
