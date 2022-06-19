#ifndef SYMOCRAFT_WORLD_H
#define SYMOCRAFT_WORLD_H

#include "core.h"

namespace SymoCraft
{

    struct Texture;
    class Frustum;
    namespace ECS
    {
        typedef uint64 EntityId;
    }
    static ECS::EntityId player;

    namespace World{
        void Init();
        constexpr uint16 chunk_radius = 6;
        glm::ivec2 toChunkCoords(const glm::vec3& worldCoordinates);
        void CreatePlayer();

        ECS::EntityId GetPlayer();
    }
}

#endif //SYMOCRAFT_WORLD_H
