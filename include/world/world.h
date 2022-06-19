#ifndef SYMOCRAFT_WORLD_H
#define SYMOCRAFT_WORLD_H

#include "core.h"

namespace SymoCraft
{
    struct Camera;
    struct Texture;
    class Frustum;

    namespace World{
        constexpr uint16 chunk_radius = 3;
        glm::ivec2 toChunkCoords(const glm::vec3& worldCoordinates);
    }
}

#endif //SYMOCRAFT_WORLD_H
