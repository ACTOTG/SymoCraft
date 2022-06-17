#include "world/chunk.h"
#include "renderer/batch.hpp"
#include "renderer/renderer.h"
#include "core/constants.h"

namespace SymoCraft
{
    static float g_normal;
    static std::array<std::array<Vertex3D, 4>, 6> block_faces{}; // Each block contains 6 faces, which contains 4 vertices

    Block Chunk::GetLocalBlock(int x, int y, int z) {
        if (x >= k_chunk_length || x < 0 || z >= k_chunk_width || z < 0) {
            if (x >= k_chunk_length) {
                return front_neighbor->GetLocalBlock(x - k_chunk_length, y, z);
            } else if (x < 0) {
                return back_neighbor->GetLocalBlock(k_chunk_length + x, y, z);
            }

            if (z >= k_chunk_width) {
                return right_neighbor->GetLocalBlock(x, y, z - k_chunk_width);
            } else if (z < 0) {
                return left_neighbor->GetLocalBlock(x, y, k_chunk_width + z);
            }
        }
        else if (y >= k_chunk_height || y < 0)
            return BlockConstants::NULL_BLOCK;

        return local_blocks[GetLocalBlockIndex(x, y, z)];
    }

    Block Chunk::GetWorldBlock(const glm::vec3 &worldPosition) {
        glm::ivec3 localPosition = glm::floor(
                worldPosition - glm::vec3(m_chunk_coord.x * 16.0f, 0.0f, m_chunk_coord.y * 16.0f));
        return GetLocalBlock(localPosition.x, localPosition.y, localPosition.z);
    }

    bool Chunk::SetLocalBlock(int x, int y, int z, Block newBlock) {
        if (x >= k_chunk_length || x < 0 || z >= k_chunk_width || z < 0)
        {
            if (x >= k_chunk_length) {
                return front_neighbor->SetLocalBlock(x - k_chunk_length, y, z, newBlock);
            } else if (x < 0) {
                return back_neighbor->SetLocalBlock(k_chunk_length + x, y, z, newBlock);
            }

            if (z >= k_chunk_width) {
                return right_neighbor->SetLocalBlock(x, y, z - k_chunk_width, newBlock);
            } else if (z < 0) {
                return left_neighbor->SetLocalBlock(x, y, k_chunk_width + z, newBlock);
            }
        }
        else if (y >= k_chunk_height || y < 0)
            return false;


        int index = GetLocalBlockIndex(x, y, z);
        BlockFormat blockFormat = get_block(newBlock.block_id);
        local_blocks[index].block_id = newBlock.block_id;
        local_blocks[index].SetTransparency(blockFormat.m_is_transparent);
        local_blocks[index].SetIsLightSource(blockFormat.m_is_lightSource);

        return true;
    }

    bool Chunk::SetWorldBlock(const glm::vec3 &worldPosition, Block newBlock) {
        glm::ivec3 localPosition = glm::floor(
                worldPosition - glm::vec3(m_chunk_coord.x * 16.0f, 0.0f, m_chunk_coord.y * 16.0f));
        return SetLocalBlock(localPosition.x, localPosition.y, localPosition.z, newBlock);
    }

    bool Chunk::RemoveLocalBlock(int x, int y, int z) {
        if (x >= k_chunk_length || x < 0 || z >= k_chunk_width || z < 0) {
            if (x >= k_chunk_length) {
                return front_neighbor->RemoveLocalBlock(x - k_chunk_length, y, z);
            } else if (x < 0) {
                return back_neighbor->RemoveLocalBlock(k_chunk_length + x, y, z);
            }

            if (z >= k_chunk_width) {
                return right_neighbor->RemoveLocalBlock(x, y, z - k_chunk_width);
            } else if (z < 0) {
                return left_neighbor->RemoveLocalBlock(x, y, k_chunk_width + z);
            }
        } else if (y >= k_chunk_height || y < 0) {
            return false;
        }

        // Replace the block with an air block
        int index = SymoCraft::Chunk::GetLocalBlockIndex(x, y, z);
        local_blocks[index].block_id = BlockConstants::AIR_BLOCK.block_id;
        local_blocks[index].SetLightColor(glm::ivec3(255, 255, 255));
        local_blocks[index].SetTransparency(true);
        local_blocks[index].SetIsLightSource(false);

        return true;
    }

    bool Chunk::RemoveWorldBlock(const glm::vec3 &worldPosition) {
        glm::ivec3 localPosition = glm::floor(
                worldPosition - glm::vec3(m_chunk_coord.x * 16.0f, 0.0f, m_chunk_coord.y * 16.0f));
        return RemoveLocalBlock(localPosition.x, localPosition.y, localPosition.z);
    }

//    SubChunk* Chunk::getSubChunk(SubChunk* subChunks, SubChunk* currentSubChunk, int currentLevel, const glm::ivec2& chunkCoordinates, bool isBlendableSubChunk)
//    {
//        bool needsNewChunk = currentSubChunk == nullptr
//                             || currentSubChunk->subChunkLevel != currentLevel
//                             || currentSubChunk->vertex_amount + 6 >= World::MaxVertsPerSubChunk
//                             || currentSubChunk->state != SubChunkState::TesselatingVertices;
//
//        SubChunk* ret = currentSubChunk;
//        if (needsNewChunk)
//        {
//            if (!subChunks->empty())
//            {
//                DebugStats::totalChunkRamUsed = DebugStats::totalChunkRamUsed + (World::MaxVertsPerSubChunk * sizeof(Vertex));
//
//                ret = subChunks->getNewPool();
//                ret->state = SubChunkState::TesselatingVertices;
//                ret->subChunkLevel = currentLevel;
//                ret->chunkCoordinates = chunkCoordinates;
//                ret->IsBlendable = isBlendableSubChunk;
//            }
//            else
//            {
//                AmoLogger_Warning("Ran out of sub-chunk vertex room.");
//                ret = nullptr;
//            }
//        }
//        return ret;
//    }

    constexpr float maxBiomeHeight = 145.0f;
    constexpr float minBiomeHeight = 55.0f;
    constexpr int oceanLevel = 9;
    constexpr uint16 maxHeight = 13;
    constexpr uint16 stoneHeight = 8;
    constexpr bool isCave = false;

    void Chunk::generateTerrain() {
        AmoBase::AmoMemory_ZeroMem(local_blocks, sizeof(Block) * k_chunk_width * k_chunk_height * k_chunk_length);

        for (int x = 0; x < k_chunk_length; x++) {
            for (int z = 0; z < k_chunk_width; z++) {
                // int16 maxHeight = TerrainGenerator::getHeight(x + worldChunkX, z + worldChunkZ, minBiomeHeight, maxBiomeHeight);
                // int16 stoneHeight = (int16)(maxHeight - 3.0f);

                for (int y = 0; y < k_chunk_height; y++) {
                    // bool isCave = TerrainGenerator::getIsCave(x + worldChunkX, y, z + worldChunkZ, maxHeight);
                    const int block_index = GetLocalBlockIndex(x, y, z);
                    if(abs(m_chunk_coord.x) > 1 || abs(m_chunk_coord.y) > 1)
                    {
                        local_blocks[block_index].block_id = BlockConstants::AIR_BLOCK.block_id;
                        local_blocks[block_index].SetTransparency(true);
                        local_blocks[block_index].SetBlendability(false);
                        local_blocks[block_index].SetIsLightSource(false);
                        local_blocks[block_index].SetLightColor(glm::ivec3(255, 255, 255));
                        continue;
                    };

                    if (!isCave) {
                        if (y == 0) {
                            // Bedrock
                            local_blocks[block_index].block_id = 5;
                            // Set the first bit of compressed data to false, to let us know
                            // this is not a transparent block
                            local_blocks[block_index].SetTransparency(false);
                            local_blocks[block_index].SetBlendability(false);
                            local_blocks[block_index].SetIsLightSource(false);
                        } else if (y < stoneHeight) {
                            // Stone
                            local_blocks[block_index].block_id = 5;
                            local_blocks[block_index].SetTransparency(false);
                            local_blocks[block_index].SetBlendability(false);
                            local_blocks[block_index].SetIsLightSource(false);
                        } else if (y < maxHeight) {
                            // Dirt
                            local_blocks[block_index].block_id = 4;
                            local_blocks[block_index].SetTransparency(false);
                            local_blocks[block_index].SetBlendability(false);
                            local_blocks[block_index].SetIsLightSource(false);
                        } else if (y == maxHeight) {
                            if (maxHeight < oceanLevel + 2) {
                                // Sand
                                local_blocks[block_index].block_id = 3;
                                local_blocks[block_index].SetTransparency(false);
                                local_blocks[block_index].SetBlendability(false);
                                local_blocks[block_index].SetIsLightSource(false);
                            } else {
                                // Grass
                                local_blocks[block_index].block_id = 2;
                                local_blocks[block_index].SetTransparency(false);
                                local_blocks[block_index].SetBlendability(false);
                                local_blocks[block_index].SetIsLightSource(false);
                            }
                        } else if (y >= minBiomeHeight && y < oceanLevel) {
                            // Water
                            local_blocks[block_index].block_id = 2;
                            local_blocks[block_index].SetTransparency(true);
                            local_blocks[block_index].SetBlendability(true);
                            local_blocks[block_index].SetIsLightSource(false);
                        } else if (!local_blocks[block_index].block_id) {
                            local_blocks[block_index].block_id = BlockConstants::AIR_BLOCK.block_id;
                            local_blocks[block_index].SetTransparency(true);
                            local_blocks[block_index].SetBlendability(false);
                            local_blocks[block_index].SetIsLightSource(false);
                        }
                    } else {
                        local_blocks[block_index].block_id = BlockConstants::AIR_BLOCK.block_id;
                        local_blocks[block_index].SetTransparency(true);
                        local_blocks[block_index].SetBlendability(false);
                        local_blocks[block_index].SetIsLightSource(false);
                        local_blocks[block_index].SetLightColor(glm::ivec3(255, 255, 255));
                    }
                }
            }
        }
    }

    void Chunk::generateRenderData() {
        SubChunk *solidSubChunk = nullptr;
        SubChunk *blendableSubChunk = nullptr;

        const int kWorldChunkX = m_chunk_coord.x * 16;
        const int kWorldChunkZ = m_chunk_coord.y * 16;

        for (int y = 0; y < k_chunk_height; y++) {
            int currentLevel = y / 16;

            for (int x = 0; x < k_chunk_length; x++) {
                for (int z = 0; z < k_chunk_width; z++) {

                    // 36 Vertices per cube
                    const Block &block = GetLocalBlock(x, y, z);

                    if (block == BlockConstants::NULL_BLOCK || block == BlockConstants::AIR_BLOCK) {
                        continue;
                    }

                    const BlockFormat &block_format = get_block(block.block_id);

                    // The order of coordinates is FRONT, RIGHT, BACK, LEFT, TOP, BOTTOM neighbor_blocks to check
                    const int neighbor_block_Xcoords[6] = {x + 1,     x, x - 1,     x,     x,     x};
                    const int neighbor_block_Ycoords[6] = {    y,     y,     y,     y, y + 1, y - 1};
                    const int neighbor_block_Zcoords[6] = {    z, z + 1,     z, z - 1,      z,    z};

                    // The 6 neighbor blocks that the target block is facing
                    Block neighbor_blocks[6];
                    // glm::ivec3 lightColors[6];

                    uint16 i;

                    for (i = 0; auto &neighbor_block: neighbor_blocks) {
                        neighbor_block = GetLocalBlock(neighbor_block_Xcoords[i],neighbor_block_Ycoords[i], neighbor_block_Zcoords[i]);
                        // lightColors[i] = neighbor_blocks[i].GetCompressedLightColor();
                        i++;
                    }

                    SubChunk **currentSubChunkPtr = &solidSubChunk;
                    if (block_format.m_is_blendable) {
                        currentSubChunkPtr = &blendableSubChunk;
                    }

                    // Only add the faces that are not culled by other neighbor_blocks
                    // Use the 6 blocks to iterate through the 6 faces
                    for (i = 0; auto &neighbor_block: neighbor_blocks) {
                        // If neighbor block is not null and is transparent
                        if (neighbor_block != BlockConstants::NULL_BLOCK && neighbor_block.IsTransparent()) {

                            //If the face aren't culled, calculate its 4 vertices
                            for( int j = 0; j < 4; j++) {
                                block_faces[i][j].pos_coord = (glm::ivec3(x + kWorldChunkX, y, z + kWorldChunkZ) +
                                        BlockConstants::pos_coords[BlockConstants::vertex_indices[i * 4 + j]]);
                                block_faces[i][j].tex_coord = {BlockConstants::tex_coords[j % 4], // Set uv coords
                                                                (i * 4 + j >= 16) ? ((i * 4 + j >= 20)
                                                                 ? // Set layer i, sides first, the top second, the bottom last
                                                                 block_format.m_bottom_texture
                                                                 : block_format.m_top_texture) // if 16 <= i < 20, assign top_tex
                                                                 : block_format.m_side_texture}; // if i < 16, assign side_tex
                                block_faces[i][j].normal = g_normal;
                            }
//                          Smooth lighting
//                          glm::vec<4, uint8, glm::defaultp> smoothLightVertex[6] = {};
//                          glm::vec<4, uint8, glm::defaultp> smoothSkyLightVertex[6] = {};
//
//                          for (int v = 0; v < 4; v++) {
//                          glm::ivec3 v0 = block_faces[i % 6][v % 4].pos_coord;
//                          glm::ivec3 v1 = block_faces[i % 6][v % 4].pos_coord;
//                          glm::ivec3 v2 = block_faces[i % 6][v % 4].pos_coord;
//                          glm::ivec3 v3 = block_faces[i % 6][v % 4].pos_coord;
//                          // GetLightVerticesBySide(i, v0, v1, v2, v3);
//
//                          const Block &v0b = GetInternalBlock(chunk, v0.x, v0.y, v0.z);
//                          const Block &v1b = GetInternalBlock(chunk, v1.x, v1.y, v1.z);
//                          const Block &v2b = GetInternalBlock(chunk, v2.x, v2.y, v2.z);
//                          const Block &v3b = GetInternalBlock(chunk, v3.x, v3.y, v3.z);
//
//                          uint8 count = 0;
//
//                          uint8 currentVertexLight = 0;
//                          uint8 currentVertexSkyLight = 0;
//
//                          if (v0b == BlockMap::NULL_BLOCK || v0b == BlockMap::AIR_BLOCK) {
//                              currentVertexLight += v0b.calculatedLightLevel();
//                              currentVertexSkyLight += v0b.calculatedSkyLightLevel();
//                              count++;
//                          }
//
//                          if (v1b == BlockMap::NULL_BLOCK || v1b == BlockMap::AIR_BLOCK) {
//                              currentVertexLight += v1b.calculatedLightLevel();
//                              currentVertexSkyLight += v1b.calculatedSkyLightLevel();
//                              count++;
//                          }
//
//                          if (v2b == BlockMap::NULL_BLOCK || v2b == BlockMap::AIR_BLOCK) {
//                              currentVertexLight += v2b.calculatedLightLevel();
//                              currentVertexSkyLight += v2b.calculatedSkyLightLevel();
//                              count++;
//                          }
//
//                          if (v3b == BlockMap::NULL_BLOCK || v3b == BlockMap::AIR_BLOCK) {
//                              currentVertexLight += v3b.calculatedLightLevel();
//                              currentVertexSkyLight += v3b.calculatedSkyLightLevel();
//                              count++;
//                          }
//
//                          if (count > 0) {
//                              currentVertexLight /= count;
//                              currentVertexSkyLight /= count;
//                          }
//
//                          smoothLightVertex[i][v] = currentVertexLight;
//                          smoothSkyLightVertex[i][v] = currentVertexSkyLight;


//                            *currentSubChunkPtr = getSubChunk(subChunks, *currentSubChunkPtr, currentLevel,
//                                                              chunk_coord, currentBlockIsBlendable);
//                            SubChunk *currentSubChunk = *currentSubChunkPtr;
//
//                            if (!currentSubChunk)
//                                break;

//                          bool colorByBiome = i == (int) CUBE_FACE::TOP
//                                                 ? block_format.colorTopByBiome
//                                                 : i == (int) CUBE_FACE::BOTTOM
//                                                   ? block_format.colorBottomByBiome
//                                                   : block_format.colorSideByBiome;

//                          loadBlock(currentSubChunk->data + currentSubChunk->numVertsUsed,
//                              vertex_positions[vertIndices[i][0]],
//                              vertex_positions[vertIndices[i][1]],
//                              vertex_positions[vertIndices[i][2]],
//                              vertex_positions[vertIndices[i][3]],
//                              *textures[i],
//                              (CUBE_FACE) i,
//                              colorByBiome,
//                              smoothLightVertex[i],
//                              smoothSkyLightVertex[i],
//                                  lightColors[i]);

                        // Add the block's top left triangle
                        block_batch.AddVertex(block_faces[i][0]);
                        block_batch.AddVertex(block_faces[i][1]);
                        block_batch.AddVertex(block_faces[i][2]);

                        // Add the block's bottom right triangle
                        block_batch.AddVertex(block_faces[i][0]);
                        block_batch.AddVertex(block_faces[i][2]);
                        block_batch.AddVertex(block_faces[i][3]);

                        vertex_count += 6;
                        face_count += 1;
//                            currentSubChunk->vertex_amount += 6;
                        }
                        i++;
                    }
                }
            }
        }
    }


//    void Block::LoadBlockFace(
//            Vertex* vertexData,
//            const std::array<Vertex3D, 4> &block_face,
//            CUBE_FACE face,
//            bool colorFaceBasedOnBiome,
//            glm::vec<4, uint8, glm::defaultp>& lightLevels,
//            glm::vec<4, uint8, glm::defaultp>& skyLightLevels,
//            const glm::ivec3& lightColor){
//        UV_INDEX uv0 = UV_INDEX::BOTTOM_RIGHT;
//        UV_INDEX uv1 = UV_INDEX::TOP_RIGHT;
//        UV_INDEX uv2 = UV_INDEX::TOP_LEFT;
//
//        UV_INDEX uv3 = UV_INDEX::BOTTOM_RIGHT;
//        UV_INDEX uv4 = UV_INDEX::TOP_LEFT;
//        UV_INDEX uv5 = UV_INDEX::BOTTOM_LEFT;
//
//        switch (face)
//        {
//            case CUBE_FACE::BACK:
//                uv0 = (UV_INDEX)(((int)uv0 + 2) % (int)UV_INDEX::SIZE);
//                uv1 = (UV_INDEX)(((int)uv1 + 2) % (int)UV_INDEX::SIZE);
//                uv2 = (UV_INDEX)(((int)uv2 + 2) % (int)UV_INDEX::SIZE);
//                uv3 = (UV_INDEX)(((int)uv3 + 2) % (int)UV_INDEX::SIZE);
//                uv4 = (UV_INDEX)(((int)uv4 + 2) % (int)UV_INDEX::SIZE);
//                uv5 = (UV_INDEX)(((int)uv5 + 2) % (int)UV_INDEX::SIZE);
//                break;
//            case CUBE_FACE::RIGHT:
//                uv0 = (UV_INDEX)(((int)uv0 + 3) % (int)UV_INDEX::SIZE);
//                uv1 = (UV_INDEX)(((int)uv1 + 3) % (int)UV_INDEX::SIZE);
//                uv2 = (UV_INDEX)(((int)uv2 + 3) % (int)UV_INDEX::SIZE);
//                uv3 = (UV_INDEX)(((int)uv3 + 3) % (int)UV_INDEX::SIZE);
//                uv4 = (UV_INDEX)(((int)uv4 + 3) % (int)UV_INDEX::SIZE);
//                uv5 = (UV_INDEX)(((int)uv5 + 3) % (int)UV_INDEX::SIZE);
//                break;
//            case CUBE_FACE::LEFT:
//                uv0 = (UV_INDEX)(((int)uv0 + 3) % (int)UV_INDEX::SIZE);
//                uv1 = (UV_INDEX)(((int)uv1 + 3) % (int)UV_INDEX::SIZE);
//                uv2 = (UV_INDEX)(((int)uv2 + 3) % (int)UV_INDEX::SIZE);
//                uv3 = (UV_INDEX)(((int)uv3 + 3) % (int)UV_INDEX::SIZE);
//                uv4 = (UV_INDEX)(((int)uv4 + 3) % (int)UV_INDEX::SIZE);
//                uv5 = (UV_INDEX)(((int)uv5 + 3) % (int)UV_INDEX::SIZE);
//                break;
//        }
//
//        vertexData[0] = compress(vert1, texture, face, uv0, colorFaceBasedOnBiome, lightLevels[0], lightColor, skyLightLevels[0]);
//        vertexData[1] = compress(vert2, texture, face, uv1, colorFaceBasedOnBiome, lightLevels[1], lightColor, skyLightLevels[1]);
//        vertexData[2] = compress(vert3, texture, face, uv2, colorFaceBasedOnBiome, lightLevels[2], lightColor, skyLightLevels[2]);
//
//        vertexData[3] = compress(vert1, texture, face, uv3, colorFaceBasedOnBiome, lightLevels[0], lightColor, skyLightLevels[0]);
//        vertexData[4] = compress(vert3, texture, face, uv4, colorFaceBasedOnBiome, lightLevels[2], lightColor, skyLightLevels[2]);
//        vertexData[5] = compress(vert4, texture, face, uv5, colorFaceBasedOnBiome, lightLevels[3], lightColor, skyLightLevels[3]);
//   }
}