#include <fast_noise_lite/FastNoiseLite.h>
#include <random>
#include "world/chunk.h"
#include "world/world.h"
#include "renderer/renderer.h"
#include "core/constants.h"
#include "core/utils.h"


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

    Block Chunk::GetWorldBlock(const glm::vec3 &world_coord) {
        glm::ivec3 localPosition = glm::floor(
                world_coord - glm::vec3(m_chunk_coord.x * 16.0f, 0.0f, m_chunk_coord.y * 16.0f));
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

        UpdateChunkLocalBlocks({x, y, z});
        return true;
    }

    bool Chunk::SetWorldBlock(const glm::vec3 &world_coord, Block new_block) {
        glm::ivec3 localPosition = glm::floor(
                world_coord - glm::vec3(m_chunk_coord.x * 16.0f, 0.0f, m_chunk_coord.y * 16.0f));
        return SetLocalBlock(localPosition.x, localPosition.y, localPosition.z, new_block);
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

        UpdateChunkLocalBlocks({x, y, z});
        return true;
    }

    bool Chunk::RemoveWorldBlock(const glm::vec3 &world_coord) {
        glm::ivec3 localPosition = glm::floor(
                world_coord - glm::vec3(m_chunk_coord.x * 16.0f, 0.0f, m_chunk_coord.y * 16.0f));
        return RemoveLocalBlock(localPosition.x, localPosition.y, localPosition.z);
    }

    void InitializeNoise() {
        seed = mt();

        for(auto& noise_generator : noise_generators)
        {
            noise_generator.noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise_generator.noise.SetFractalType(FastNoiseLite::FractalType_FBm);
            noise_generator.noise.SetFractalOctaves(8);
            noise_generator.noise.SetFractalLacunarity(1.6f);
            noise_generator.noise.SetSeed(static_cast<int>(mt()));
        }

        noise_generators[0].noise.SetFrequency(0.005);
        noise_generators[1].noise.SetFrequency(0.02);
        noise_generators[2].noise.SetFrequency(0.1);

        noise_generators[0].weight = 1.0f;
        noise_generators[1].weight = 0.3f;
        noise_generators[2].weight = 0.1f;

        for(auto& noise_generator : noise_generators)
            weight_sum += noise_generator.weight;
    }

    static float max_range{1};
    static float min_range{0};
    float Chunk::GetNoise(int x, int z)
    {
        float blended_noise{0};
        for(auto& noise_generator : noise_generators)
        {
            blended_noise += Remap(noise_generator.noise.GetNoise((float)x / 1.5f, (float)z / 1.5f),
                                   -1.0f, 1.0f, 0.0f, 1.0f) * noise_generator.weight;
        }

        max_range = std::max(max_range, blended_noise);
        min_range = std::min(min_range, blended_noise);
        blended_noise /= weight_sum;
        blended_noise = pow(blended_noise, 1.19f);
        return Remap(blended_noise, 0.0f, 1.0f, minBiomeHeight, maxBiomeHeight);
    }

    void Report()
    {
        AmoLogger_Info("The range is between: %d - %d\n The seed is %d", (int)min_range, (int)max_range, seed);
    }

    void Chunk::GenerateTerrain() {
        AmoBase::AmoMemory_ZeroMem(local_blocks, sizeof(Block) * k_chunk_width * k_chunk_height * k_chunk_length);

        int world_x = m_chunk_coord.x * k_chunk_length;
        int world_z = m_chunk_coord.y * k_chunk_width;
        for (int z = 0; z < k_chunk_width; z++) {
            for (int x = 0; x < k_chunk_length; x++) {
                maxHeight = (uint16)GetNoise(x + world_x, z + world_z);
                stoneHeight = maxHeight - 6;

                for (int y = 0; y < k_chunk_height; y++) {
                    // bool isCave = TerrainGenerator::getIsCave(x + worldChunkX, y, z + worldChunkZ, maxHeight);
                    const int block_index = GetLocalBlockIndex(x , y, z);
                    if(abs(m_chunk_coord.x) > World::chunk_radius - 1|| abs(m_chunk_coord.y) > World::chunk_radius - 1)
                    {
                        local_blocks[block_index].block_id = BlockConstants::AIR_BLOCK.block_id;
                        local_blocks[block_index].SetTransparency(true);
                        local_blocks[block_index].SetBlendability(false);
                        local_blocks[block_index].SetIsLightSource(false);
                        local_blocks[block_index].SetLightColor(glm::ivec3(255, 255, 255));
                        continue;
                    };

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
                        } else if (y < minBiomeHeight - 3) {
                            // Dirt
                            local_blocks[block_index].block_id = 4;
                            local_blocks[block_index].SetTransparency(false);
                            local_blocks[block_index].SetBlendability(false);
                            local_blocks[block_index].SetIsLightSource(false);
                        } else if ( minBiomeHeight - 3 < y < minBiomeHeight + 3 ) {
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
                            local_blocks[block_index].block_id = 4;
                            local_blocks[block_index].SetTransparency(false);
                            local_blocks[block_index].SetBlendability(true);
                            local_blocks[block_index].SetIsLightSource(false);
                        } else if (!local_blocks[block_index].block_id) {
                            local_blocks[block_index].block_id = BlockConstants::AIR_BLOCK.block_id;
                            local_blocks[block_index].SetTransparency(true);
                            local_blocks[block_index].SetBlendability(false);
                            local_blocks[block_index].SetIsLightSource(false);
                        }
                }
//                    else {
//                        local_blocks[block_index].block_id = BlockConstants::AIR_BLOCK.block_id;
//                        local_blocks[block_index].SetTransparency(true);
//                        local_blocks[block_index].SetBlendability(false);
//                        local_blocks[block_index].SetIsLightSource(false);
//                        local_blocks[block_index].SetLightColor(glm::ivec3(255, 255, 255));

            }

        }

    }

    void Chunk::GenerateVegetation()
    {
           const int worldChunkX = m_chunk_coord.x * 16;
           const int worldChunkZ = m_chunk_coord.y * 16;

//           glm::ivec2 localChunkPos = glm::vec2(lastPlayerLoadPosChunkCoords.x - chunkX, lastPlayerLoadPosChunkCoords.y - chunkZ);
//           bool inRangeOfPlayer =
//                   (localChunkPos.x * localChunkPos.x) + (localChunkPos.y * localChunkPos.y) <=
//                   ((World::chunk_radius - 1) * (World::chunk_radius - 1));
//           if (!inRangeOfPlayer)
//           {
//               // Skip over all chunks in range radius - 1
//               continue;
//           }

           for (int x = 0; x < World::chunk_radius; x++)
           {
               for (int z = 0; z < World::chunk_radius; z++)
               {
                   // Generate some trees if needed
                   bool generateTree = mt() % 100 > 98;

                   if (generateTree)
                   {
                       uint16 y = GetNoise(x + worldChunkX, z + worldChunkZ) + 1;

                       if (y > oceanLevel + 2)
                       {
                           // Generate a tree
                           uint16 treeHeight = (mt() % 3) + 3;
                           uint16 leavesBottomY = glm::clamp(treeHeight - 3, 3, (int)k_chunk_height - 1);
                           uint16 leavesTopY = treeHeight + 1;
                           if (y + 1 + leavesTopY < k_chunk_height)
                           {
                               for (int treeY = 0; treeY <= treeHeight; treeY++)
                               {
                                   local_blocks[GetLocalBlockIndex(x, treeY + y, z)].block_id = 6;
                                   local_blocks[GetLocalBlockIndex(x, treeY + y, z)].SetBlendability(false);
                                   local_blocks[GetLocalBlockIndex(x, treeY + y, z)].SetTransparency(false);
                                   local_blocks[GetLocalBlockIndex(x, treeY + y, z)].SetIsLightSource(false);
                               }

                               int ringLevel = 0;
                               for (int leavesY = leavesBottomY + y; leavesY <= leavesTopY + y; leavesY++)
                               {
                                   int leafRadius = leavesY == leavesTopY ? 2 : 1;
                                   for (int leavesX = x - leafRadius; leavesX <= x + leafRadius; leavesX++)
                                   {
                                       for (int leavesZ = z - leafRadius; leavesZ <= z + leafRadius; leavesZ++)
                                       {
                                           if (leavesX < k_chunk_length && leavesX >= 0 && leavesZ < k_chunk_width && leavesZ >= 0)
                                           {
                                               local_blocks[GetLocalBlockIndex(leavesX, leavesY, leavesZ)].block_id = 7;
                                               local_blocks[GetLocalBlockIndex(leavesX, leavesY, leavesZ)].SetBlendability(false);
                                               local_blocks[GetLocalBlockIndex(leavesX, leavesY, leavesZ)].SetTransparency(true);
                                               local_blocks[GetLocalBlockIndex(leavesX, leavesY, leavesZ)].SetIsLightSource(false);
                                           }
                                           else if (leavesX < 0)
                                           {
                                               if (back_neighbor)
                                               {
                                                   back_neighbor->local_blocks[GetLocalBlockIndex(k_chunk_length + leavesX, leavesY, leavesZ)].block_id = 7;
                                                   local_blocks[GetLocalBlockIndex(k_chunk_length + leavesX, leavesY, leavesZ)].SetBlendability(false);
                                                   local_blocks[GetLocalBlockIndex(k_chunk_length + leavesX, leavesY, leavesZ)].SetTransparency(true);
                                                   local_blocks[GetLocalBlockIndex(k_chunk_length + leavesX, leavesY, leavesZ)].SetIsLightSource(false);
                                               }
                                           }
                                           else if (leavesX >= k_chunk_length)
                                           {
                                               if (front_neighbor)
                                               {
                                                   front_neighbor->local_blocks[GetLocalBlockIndex(leavesX - k_chunk_length, leavesY, leavesZ)].block_id = 7;
                                                   local_blocks[GetLocalBlockIndex(leavesX - k_chunk_length, leavesY, leavesZ)].SetBlendability(false);
                                                   local_blocks[GetLocalBlockIndex(leavesX - k_chunk_length, leavesY, leavesZ)].SetTransparency(true);
                                                   local_blocks[GetLocalBlockIndex(leavesX - k_chunk_length, leavesY, leavesZ)].SetIsLightSource(false);
                                               }
                                           }
                                           else if (leavesZ < 0)
                                           {
                                               if (left_neighbor)
                                               {
                                                   left_neighbor->local_blocks[GetLocalBlockIndex(leavesX, leavesY, k_chunk_width + leavesZ)].block_id = 7;
                                                   local_blocks[GetLocalBlockIndex(leavesX, leavesY, k_chunk_width + leavesZ)].SetBlendability(false);
                                                   local_blocks[GetLocalBlockIndex(leavesX, leavesY, k_chunk_width + leavesZ)].SetTransparency(true);
                                                   local_blocks[GetLocalBlockIndex(leavesX, leavesY, k_chunk_width + leavesZ)].SetIsLightSource(false);
                                               }
                                           }
                                           else if (leavesZ >= k_chunk_width)
                                           {
                                               if (right_neighbor)
                                               {
                                                   right_neighbor->local_blocks[GetLocalBlockIndex(leavesX, leavesY, leavesZ - k_chunk_width)].block_id = 7;
                                                   local_blocks[GetLocalBlockIndex(leavesX, leavesY, leavesZ - k_chunk_width)].SetBlendability(false);
                                                   local_blocks[GetLocalBlockIndex(leavesX, leavesY, leavesZ - k_chunk_width)].SetTransparency(true);
                                                   local_blocks[GetLocalBlockIndex(leavesX, leavesY, leavesZ - k_chunk_width)].SetIsLightSource(false);
                                               }
                                           }
                                       }
                                   }
                                   ringLevel++;
                               }
                           }
                       }
                   }
               }
           }


    }

    void Chunk::GenerateRenderData() {

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

                    // Only add the faces that are not culled by other neighbor_blocks
                    // Use the 6 blocks to iterate through the 6 faces
                    for (i = 0; auto &neighbor_block: neighbor_blocks) {
                        // If neighbor block is not null and is transparent
                        if ((neighbor_block != BlockConstants::NULL_BLOCK && neighbor_block.IsTransparent()) || block.block_id == 7) {

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
                        }
                        i++;
                    }
                }
            }
        }
    }

    void Chunk::UpdateChunkLocalBlocks(const glm::vec3& block_world_coord)
    {
        glm::ivec3 block_local_coord = glm::floor(block_world_coord - glm::vec3(m_chunk_coord.x * 16.0f, 0.0f, m_chunk_coord.y * 16.0f));

        if (block_local_coord.x == 0)
        {
            if (back_neighbor)
            {
                back_neighbor->state = ChunkState::ToBeUpdated;
            }
        }
        else if (block_local_coord.x == 15)
        {
            if (front_neighbor)
            {
                front_neighbor->state = ChunkState::ToBeUpdated;
            }
        }
        if (block_local_coord.z == 0)
        {
            if (left_neighbor)
            {
                left_neighbor->state = ChunkState::ToBeUpdated;
            }
        }
        else if (block_local_coord.z == 15)
        {
            if (right_neighbor)
            {
                right_neighbor->state = ChunkState::ToBeUpdated;
            }
        }
    }
}