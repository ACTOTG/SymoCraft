#ifndef SYMOCRAFT_CHUNK_MANAGER_H
#define SYMOCRAFT_CHUNK_MANAGER_H

#include "core.h"
#include "world.h"
#include "renderer/shader_program.h"


namespace SymoCraft{
    class Block;
    class Frustum;
    class Chunk;
    enum class ChunkState : uint8;

    // A chunk is 16 * 16 * 256
    static constexpr uint16 k_chunk_length = 16;
    static constexpr uint16 k_chunk_width = 16;
    static constexpr uint16 k_chunk_height = 128;

    static constexpr int maxBiomeHeight = 70;
    static constexpr int minBiomeHeight = 40;
    static constexpr int oceanLevel = 48;

    struct Vertex
    {
        uint32 data1;
        uint32 data2;
    };


    namespace ChunkManager
    {
        void init();
        void free();
        void serialize();
        void serializeSynchronous();

        robin_hood::unordered_node_map<glm::ivec2, Chunk>& getAllChunks();

        float percentWorkDone();
        Block GetBlock(const glm::vec3& worldPosition);
        void SetBlock(const glm::vec3& worldPosition, Block newBlock);
        void RemoveBLock(const glm::vec3& worldPosition);

        Chunk* GetChunk(const glm::vec3& worldPosition);
        Chunk* GetChunk(const glm::ivec2& chunkCoords);

        void RearrangeChunkPointers();
        void UpdateChunkLocalBlocks();
        void beginWork();
        void wakeUpCv2();
        void SetPlayerChunkCoord(const glm::ivec2& player_chunk_coord);

        void render(const glm::vec3& playerPosition, const glm::ivec2& playerPositionInChunkCoords, Shader& opaqueShader, Shader& transparentShader, const Frustum& cameraFrustum);
        void checkChunkRadius(const glm::vec3& playerPosition, bool isClient=false);

        void queueClientLoadChunk(void* chunkData, const glm::ivec2& chunkCoordinates, ChunkState state);
        void queueGenerateDecorations(const glm::ivec2& lastPlayerLoadChunkPos);
        void queueCalculateLighting(const glm::ivec2& lastPlayerPosInChunkCoords);
        void queueCreateChunk(const glm::ivec2& chunkCoordinates);
        void queueRecalculateLighting(const glm::ivec2& chunkCoordinates, const glm::vec3& blockPositionThatUpdated, bool removedLightSource);
        void queueRetesselateChunk(const glm::ivec2& chunkCoordinates, Chunk* chunk = nullptr);
    }
}
#endif //SYMOCRAFT_CHUNK_MANAGER_H
