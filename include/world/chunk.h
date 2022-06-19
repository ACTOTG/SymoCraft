#ifndef SYMOCRAFT_CHUNK_H
#define SYMOCRAFT_CHUNK_H

#include <fast_noise_lite/FastNoiseLite.h>
#include "core.h"
#include "block.h"
#include "chunk_manager.h"
#include "renderer/batch.hpp"

namespace SymoCraft {

    enum class ChunkState : uint8 {
        None,
        Unloaded,
        Unloading,
        ToBeUpdated,
        Updated,
        Saving,
        Loading,
        Loaded
    };

    struct NoiseGenerator
    {
        FastNoiseLite noise;
        float weight;
    };

    static uint16 maxHeight;
    static uint16 stoneHeight;
    static uint32 seed;
    static float weight_sum;
    static std::array<NoiseGenerator, 3> noise_generators{};
    static std::mt19937 mt{ std::random_device{}() };

    static std::array<std::array<float, k_chunk_width>, k_chunk_length> height_map;
    void InitializeNoise();
    void Report();

    class Chunk {
    public:
        Block *local_blocks;
        glm::ivec2 m_chunk_coord;
        ChunkState state;
        bool needsToGenerateDecorations;
        bool needsToCalculateLighting;

        Chunk* front_neighbor;
        Chunk* back_neighbor;
        Chunk* left_neighbor;
        Chunk* right_neighbor;

        inline bool operator==(const Chunk &other) const {
            return m_chunk_coord == other.m_chunk_coord;
        }

        inline bool operator!=(const Chunk &other) const {
            return m_chunk_coord != other.m_chunk_coord;
        }

        inline bool operator==(const glm::ivec2 &other) const {
            return m_chunk_coord == other;
        }

        inline bool operator!=(const glm::ivec2 &other) const {
            return m_chunk_coord != other;
        }

        struct HashFunction {
            inline std::size_t operator()(const Chunk &key) const {
                return std::hash<int>()(key.m_chunk_coord.x) ^
                       std::hash<int>()(key.m_chunk_coord.y);
            }
        };

        Block GetWorldBlock(const glm::vec3 &world_coord);
        bool SetWorldBlock(const glm::vec3 &world_coord, Block new_block);
        bool RemoveWorldBlock(const glm::vec3 &world_coord);

        float GetNoise(int x, int z);

        void GenerateTerrain();
        void GenerateVegetation();
        void GenerateRenderData();
        void UpdateChunkLocalBlocks(const glm::vec3& block_world_coord);

    private:
        Block GetLocalBlock(int x, int y, int z);
        bool SetLocalBlock(int x, int y, int z, Block newBlock);
        bool RemoveLocalBlock(int x, int y, int z);

        // Must guarantee at least 16 sub-chunks located at this address

        //void calculateLighting(const glm::ivec2 &lastPlayerLoadPosChunkCoords);
        //void calculateLightingUpdate(Chunk *chunk, const glm::ivec2 &chunkCoordinates, const glm::vec3 &blockPosition,
        //                            bool removedLightSource,
        //                            robin_hood::unordered_flat_set<Chunk *> &chunksToRetesselate);

        // void serialize(const std::string &worldSavePath, const Chunk &chunk);
        // void deserialize(Chunk &blockData, const std::string &worldSavePath);
        // bool exists(const std::string &worldSavePath, const glm::ivec2 &chunkCoordinates);

        // void info();

        inline int GetLocalBlockIndex(int x, int y ,int z){
            return x * k_chunk_length + y * k_chunk_height + z;
        }

    };
}



#endif //SYMOCRAFT_CHUNK_H
