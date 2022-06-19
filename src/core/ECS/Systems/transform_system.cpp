//
// Created by Amo on 2022/6/18.
//
#include "core/ECS/Systems/transform_system.h"
#include "core.h"
#include "core/ECS/component.h"

namespace SymoCraft
{
    namespace TransformSystem
    {
        void Update(ECS::Registry &registry)
        {
            for (ECS::EntityId entity : registry.View<Transform>())
            {
                Transform& transform = registry.GetComponent<Transform>(entity);
                glm::vec3 direction;
                direction.x = cos(glm::radians(transform.orientation.y)) * cos(glm::radians(transform.orientation.x));
                direction.y = sin(glm::radians(transform.orientation.x));
                direction.z = sin(glm::radians(transform.orientation.y)) * cos(glm::radians(transform.orientation.x));

                transform.front = glm::normalize(direction);
                transform.right =
            }
        }
    }
}