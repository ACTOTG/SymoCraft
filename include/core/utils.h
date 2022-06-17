//
// Created by Administrator on 2022/6/17.
//

#ifndef SYMOCRAFT_UTILS_H
#define SYMOCRAFT_UTILS_H
#include "core.h"
#include "world/block.h"

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

namespace INormals2
{
    constexpr glm::ivec2 Front = glm::ivec2(1, 0);
    constexpr glm::ivec2 Back = glm::ivec2(-1, 0);
    constexpr glm::ivec2 Left = glm::ivec2(0, -1);
    constexpr glm::ivec2 Right = glm::ivec2(0, 1);

    constexpr std::array<glm::ivec2, 4> CardinalDirections = {
            Front,
            Back,
            Left,
            Right
    };

}

#endif //SYMOCRAFT_UTILS_H
