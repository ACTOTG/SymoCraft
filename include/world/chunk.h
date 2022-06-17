#ifndef SYMOCRAFT_CHUNK_H
#define SYMOCRAFT_CHUNK_H

#include "core.h"
#include "block.h"
#include "renderer/renderer.h"
#include "renderer/batch.hpp"
#include "chunk_manager.h"

namespace SymoCraft {

    // A chunk is 16 * 16 * 256
    // A sub-chunk is 16 * 16 * 16
    constexpr uint16 k_chunk_length = 16;
    constexpr uint16 k_chunk_width = 16;
    constexpr uint16 k_chunk_height = 64;

    enum class ChunkState : uint8 {
        None,
        Unloaded,
        Unloading,
        Saving,
        Loading,
        Loaded
    };

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

        Block GetWorldBlock(const glm::vec3 &worldPosition);
        bool SetWorldBlock(const glm::vec3 &worldPosition, Block newBlock);
        bool RemoveWorldBlock(const glm::vec3 &worldPosition);


        void generateTerrain();
        void generateRenderData();

    private:
        enum class CUBE_FACE : uint32
        {
            LEFT = 0,
            RIGHT = 1,
            BOTTOM = 2,
            TOP = 3,
            BACK = 4,
            FRONT = 5,
            SIZE = 6
        };

        enum class UV_INDEX : uint32
        {
            TOP_RIGHT = 0,
            TOP_LEFT = 1,
            BOTTOM_LEFT = 2,
            BOTTOM_RIGHT = 3,
            SIZE
        };

        Block GetLocalBlock(int x, int y, int z);
        bool SetLocalBlock(int x, int y, int z, Block newBlock);
        bool RemoveLocalBlock(int x, int y, int z);

//        SubChunk* getSubChunk(SubChunk* subChunks, SubChunk* currentSubChunk, int currentLevel, const glm::ivec2& chunkCoordinates, bool isBlendableSubChunk);

        //void generateDecorations(const glm::ivec2 &lastPlayerLoadPosChunkCoords, float seed);

        // Must guarantee at least 16 sub-chunks located at this address

        //void calculateLighting(const glm::ivec2 &lastPlayerLoadPosChunkCoords);
        //void calculateLightingUpdate(Chunk *chunk, const glm::ivec2 &chunkCoordinates, const glm::vec3 &blockPosition,
        //                            bool removedLightSource,
        //                            robin_hood::unordered_flat_set<Chunk *> &chunksToRetesselate);

        // void serialize(const std::string &worldSavePath, const Chunk &chunk);
        // void deserialize(Chunk &blockData, const std::string &worldSavePath);
        // bool exists(const std::string &worldSavePath, const glm::ivec2 &chunkCoordinates);

        // void info();

        void LoadBlockFace(Vertex*, const std::array<Vertex3D, 4> &block_face, CUBE_FACE face);

        inline int GetLocalBlockIndex(int x, int y ,int z){
            return x * k_chunk_length + y * k_chunk_height + z;
        }

    };
}



#endif //SYMOCRAFT_CHUNK_H
