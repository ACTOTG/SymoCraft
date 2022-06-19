#include "world/chunk_manager.h"
#include "world/chunk.h"
#include "core/constants.h"

namespace SymoCraft{

    static std::mutex chunkMtx;
    static robin_hood::unordered_node_map<glm::ivec2, Chunk> chunks;
    static uint32 chunkPosInstancedBuffer;
    static uint32 biomeInstancedVbo;
    static uint32 globalVao;
    static uint32 globalRenderVbo;
    // TODO: Make this better
    static uint32 solidDrawCommandVbo;
    static uint32 blendableDrawCommandVbo;
    static Shader compositeShader;

//        static ChunkThreadWorker* chunkWorker = nullptr;
//        static Pool<SubChunk>* subChunks = nullptr;
//        static Pool<Block>* blockPool = nullptr;
//        static CommandBufferContainer* solidCommandBuffer = nullptr;
//        static CommandBufferContainer* blendableCommandBuffer = nullptr;

    namespace ChunkManager {
        Block getBlock(const glm::vec3 &worldPosition) {
            Chunk *chunk = getChunk(worldPosition);

            if (!chunk) {
                // Assume it's a chunk that's out of bounds
                // TODO: Make this only return null block if it's far far away from the player
                return BlockConstants::NULL_BLOCK;
            }

            return chunk->GetWorldBlock(worldPosition);
        }

//        void setBlock(const glm::vec3 &worldPosition, Block newBlock) {
//            glm::ivec2 chunkCoords = World::toChunkCoords(worldPosition);
//            Chunk *chunk = getChunk(worldPosition);
//
//            if (!chunk) {
//                if (worldPosition.y >= 0 && worldPosition.y < 256) {
//                    // Assume it's a chunk that's out of bounds
//                    AmoLogger_Warning("Tried to set invalid block at position<%2.3f, %2.3f, %2.3f>!", worldPosition.x,
//                                     worldPosition.y, worldPosition.z);
//                }
//                return;
//            }
//
//            if (chunk->SetWorldBlock(worldPosition, newBlock)) {
//                retesselateChunkBlockUpdate(chunkCoords, worldPosition, chunk);
//                queueRecalculateLighting(chunkCoords, worldPosition, false);
//                chunkWorker->beginWork();
//            }
//        }
//
//        void removeBlock(const glm::vec3 &worldPosition) {
//            glm::ivec2 chunkCoords = World::toChunkCoords(worldPosition);
//            Chunk *chunk = getChunk(worldPosition);
//
//            if (!chunk) {
//                if (worldPosition.y >= 0 && worldPosition.y < 256) {
//                    // Assume it's a chunk that's out of bounds
//                    AmoLogger_Warning("Tried to set invalid block at position<%2.3f, %2.3f, %2.3f>!", worldPosition.x,
//                                     worldPosition.y, worldPosition.z);
//                }
//                return;
//            }
//
//            bool isLightSourceBlock = ChunkManager::getBlock(worldPosition).IsLightSource();
//            if (ChunkPrivate::removeBlock(worldPosition, chunkCoords, chunk)) {
//                retesselateChunkBlockUpdate(chunkCoords, worldPosition, chunk);
//                queueRecalculateLighting(chunkCoords, worldPosition, isLightSourceBlock);
//                chunkWorker->beginWork();
//            }
//        }

        Chunk *getChunk(const glm::vec3 &worldPosition) {
            glm::ivec2 chunkCoords = World::toChunkCoords(worldPosition);
            return getChunk(chunkCoords);
        }

        Chunk *getChunk(const glm::ivec2 &chunkCoords) {
            Chunk *chunk = nullptr;

            {
                // TODO: Make this thread-safe somehow or something
                // or make sure no threads access this
                std::lock_guard<std::mutex> lock(chunkMtx);
                const robin_hood::unordered_map<glm::ivec2, Chunk>::iterator &iter = chunks.find(chunkCoords);
                if (iter != chunks.end()) {
                    chunk = &iter->second;
                }
            }

            return chunk;
        }

        robin_hood::unordered_node_map<glm::ivec2, Chunk>& getAllChunks()
        {
            return chunks;
        }

        void patchChunkPointers() {
            for (auto &pair: chunks) {
                Chunk &chunk = pair.second;
                auto iter1 = chunks.find(chunk.m_chunk_coord + INormals2::Front);
                chunk.front_neighbor = iter1 == chunks.end() ? nullptr : &iter1->second;
                auto iter2 = chunks.find(chunk.m_chunk_coord + INormals2::Back);
                chunk.back_neighbor = iter2 == chunks.end() ? nullptr : &iter2->second;
                auto iter3 = chunks.find(chunk.m_chunk_coord + INormals2::Left);
                chunk.left_neighbor = iter3 == chunks.end() ? nullptr : &iter3->second;
                auto iter4 = chunks.find(chunk.m_chunk_coord + INormals2::Right);
                chunk.right_neighbor = iter4 == chunks.end() ? nullptr : &iter4->second;
            }
        }

        void queueCreateChunk(const glm::ivec2& chunkCoordinates)
        {
            // Only upload if we need to
            Chunk* chunk = getChunk(chunkCoordinates);
            if (!chunk)
            {
//                if (!blockPool->empty())
//                {
                    Chunk newChunk;
                    newChunk.local_blocks = (Block*)AmoMemory_Allocate(sizeof(Block) * k_chunk_length * k_chunk_width * k_chunk_height);
                    newChunk.m_chunk_coord = chunkCoordinates;
                    newChunk.front_neighbor = getChunk(chunkCoordinates + INormals2::Front);
                    newChunk.back_neighbor = getChunk(chunkCoordinates + INormals2::Back);
                    newChunk.left_neighbor = getChunk(chunkCoordinates + INormals2::Left);
                    newChunk.right_neighbor = getChunk(chunkCoordinates + INormals2::Right);
                    newChunk.state = ChunkState::Loaded;

                    {
                        // TODO: Ensure this is only ever accessed from the main thread
                        //std::lock_guard lock(chunkMtx);
                        chunks[newChunk.m_chunk_coord] = newChunk;
                    }
                }
            }
        }



//        // TODO: Simplify me!
//        static void retesselateChunkBlockUpdate(const glm::ivec2& chunkCoords, const glm::vec3& worldPosition, Chunk* chunk)
//        {
//            FillChunkCommand cmd;
//            cmd.type = CommandType::TesselateVertices;
//            cmd.subChunks = subChunks;
//            cmd.chunk = chunk;
//
//            // Get any neighboring chunks that need to be updated
//            int numChunksToUpdate = 1;
//            Chunk* chunksToUpdate[3];
//            chunksToUpdate[0] = chunk;
//            glm::ivec3 localPosition = glm::floor(worldPosition - glm::vec3(chunkCoords.x * 16.0f, 0.0f, chunkCoords.y * 16.0f));
//            if (localPosition.x == 0)
//            {
//                if (chunk->bottomNeighbor)
//                {
//                    chunksToUpdate[numChunksToUpdate++] = chunk->bottomNeighbor;
//                }
//            }
//            else if (localPosition.x == 15)
//            {
//                if (chunk->topNeighbor)
//                {
//                    chunksToUpdate[numChunksToUpdate++] = chunk->topNeighbor;
//                }
//            }
//            if (localPosition.z == 0)
//            {
//                if (chunk->leftNeighbor)
//                {
//                    chunksToUpdate[numChunksToUpdate++] = chunk->leftNeighbor;
//                }
//            }
//            else if (localPosition.z == 15)
//            {
//                if (chunk->rightNeighbor)
//                {
//                    chunksToUpdate[numChunksToUpdate++] = chunk->rightNeighbor;
//                }
//            }
//
//            // Update the sub-chunks that are about to be deleted
//            for (int i = 0; i < (int)subChunks->size(); i++)
//            {
//                if ((*subChunks)[i]->state == SubChunkState::Uploaded)
//                {
//                    for (int j = 0; j < numChunksToUpdate; j++)
//                    {
//                        if ((*subChunks)[i]->chunkCoordinates == chunksToUpdate[j]->chunkCoords)
//                        {
//                            (*subChunks)[i]->state = SubChunkState::RetesselateVertices;
//                        }
//                    }
//                }
//            }
//
//            // Queue up all the chunks
//            chunkWorker->queueCommand(cmd);
//            for (int i = 1; i < numChunksToUpdate; i++)
//            {
//                cmd.chunk = chunksToUpdate[i];
//                chunkWorker->queueCommand(cmd);
//            }
//            chunkWorker->beginWork();
//        }
}