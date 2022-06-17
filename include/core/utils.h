//
// Created by Administrator on 2022/6/17.
//

#ifndef SYMOCRAFT_UTILS_H
#define SYMOCRAFT_UTILS_H
#include "core.h"
#include "world/block.h"
namespace SymoCraft{
    namespace DebugStats
    {
        extern uint32 numDrawCalls;
        extern float lastFrameTime;
        extern glm::vec3 playerPos;
        extern glm::vec3 playerOrientation;
        extern std::atomic<float> totalChunkRamUsed;
        extern float totalChunkRamAvailable;
        extern SymoCraft::Block blockLookingAt;
        extern SymoCraft::Block airBlockLookingAt;
    }
}

#endif //SYMOCRAFT_UTILS_H
