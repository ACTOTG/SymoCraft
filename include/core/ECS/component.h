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
        float yaw;
        float pitch;

        glm::vec3 front;      // x axis
        glm::vec3 up;           // y axis
        glm::vec3 right;        // z axis
    };

    namespace Physics
    {
        struct RigidBody
        {
            glm::vec3 velocity;
            glm::vec3 acceleration;
            bool on_ground;
            bool is_sensor;
            bool use_gravity;

            void zero_forces()
            {
                acceleration = glm::vec3(0.0);
                velocity = glm::vec3(0.0);
            }
        };

        struct HitBox
        {
            glm::vec3 size;
            glm::vec3 offset;
        };
    }


}

#endif //SYMOCRAFT_COMPONENT_H
