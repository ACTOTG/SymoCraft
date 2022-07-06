#ifndef SYMOCRAFT_WORLD_H
#define SYMOCRAFT_WORLD_H

#include "core.h"

namespace SymoCraft
{
    namespace ECS
    {
        typedef uint64 EntityId;
    }
    static ECS::EntityId player;

    namespace World{
        void Init();
        inline constexpr uint16 chunk_radius = 10;
        inline constexpr uint16 max_vertices_per_chunk = UINT16_MAX;
        glm::ivec2 ToChunkCoords(const glm::vec3& worldCoordinates);
        void CreatePlayer();

        ECS::EntityId GetPlayer();
    }
}

#endif //SYMOCRAFT_WORLD_H
