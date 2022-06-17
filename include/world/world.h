#ifndef SYMOCRAFT_WORLD_H
#define SYMOCRAFT_WORLD_H

#include "core.h"

namespace SymoCraft
{
    struct Camera;
    struct Texture;
    class Frustum;

    namespace World{
        glm::ivec2 toChunkCoords(const glm::vec3& worldCoordinates);
    }
}

#endif //SYMOCRAFT_WORLD_H
