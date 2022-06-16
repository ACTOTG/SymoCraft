//
// Created by Amo on 2022/6/16.
//

#ifndef SYMOCRAFT_COMPONENT_H
#define SYMOCRAFT_COMPONENT_H
#include "core.h"

namespace SymoCraft
{
    struct Transform
    {
        glm::vec3 position;     // position in world
        glm::vec3 scale;
        glm::vec3 orientation;

        glm::vec3 forward;      // x axis
        glm::vec3 up;           // y axis
        glm::vec3 right;        // z axis
    };
}

#endif //SYMOCRAFT_COMPONENT_H
