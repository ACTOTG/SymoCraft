//
// Created by Amo on 2022/6/18.
//
#include "core/ECS/Systems/physics_system.h"
#include "core/ECS/component.h"
#include "core/ECS/registry.h"
#include "core/application.h"
#include "world/world.h"
#include "world/chunk.h"
#include "input/input.h"


namespace SymoCraft
{
    struct Interval
    {
        float _min;
        float _max;
    };


    namespace Physics
    {
        // The main component of our physics system is collision
        // The remaining part of it is to deal with collision
        enum class Force : uint8
        {
            NONE = 0,
            TOP,
            BOTTOM,
            FRONT,
            BACK,
            LEFT,
            RIGHT
        };

        struct CollisionInfo
        {
            glm::vec3 overlap_part;     // overlap part of two entity
            Force force;            // collision force
            bool did_collision;     // did they really collision?
        };

        // ----------------------------------------------------------------------------------------------------------
        // some useful uniform variables in physics system
        static glm::vec3 uniform_gravity = glm::vec3(0.0, 20.0, 0.0);
        static glm::vec3 terminal_velocity = glm::vec3(50.0f, 50.0f, 50.0f);
        static const float kPhysicsUpdateRate = 1.0f / 120.0f; // 120Hz

        // ----------------------------------------------------------------------------------------------------------
        // some useful uniform function in physics system
        static void ResolveStaticCollision(ECS::EntityId entity, RigidBody &rb, Transform &transform, HitBox &hit_box);
        static CollisionInfo StaticCollisionInformation(const RigidBody& rb1, const HitBox &hb1, const Transform& tr1
                                                        , const RigidBody& rb2, const HitBox &hb2, const Transform& tr2);
        static bool IsColliding(const HitBox &hb1, const Transform &tr1
                                , const HitBox &hb2, const Transform &tr2);
        static float OverlappingAmount(const HitBox &hb1, const Transform &tr1
                                    , const HitBox &hb2, const Transform &tr2
                                       , const glm::vec3 &axis);
        static Interval GetInterval(const HitBox &box, const Transform &transform, const glm::vec3 &axis);
        static void GetQuadrantResult(const Transform &tr1, const Transform &t2,
                                      const HitBox &b2_expanded, Force x_face, Force y_face, Force z_face);
        static glm::vec3 ClosetPointOnRay(const glm::vec3 ray_origin, const glm::vec3 ray_direction
                                          , float ray_max_distance, glm::vec3 &point);

        // ----------------------------------------------------------------------------------------------------------
        // Functions Implementation

        // Resolve static collision
        // Parameters: entity id, rigid body, transform, hit box
        static void ResolveStaticCollision(ECS::EntityId entity, RigidBody &rb, Transform &transform, HitBox &hit_box)
        {
            // Get all face coordinate of the hit box
            int32 right_x = (int32) glm::ceil(transform.position.x + hit_box.size.x * 0.5f);
            int32 left_x = (int32) glm::ceil(transform.position.x - hit_box.size.x * 0.5f);
            int32 front_z = (int32) glm::ceil(transform.position.z + hit_box.size.z * 0.5f);
            int32 back_z = (int32) glm::ceil(transform.position.z - hit_box.size.z * 0.5f);
            int32 top_y = (int32) glm::ceil(transform.position.y + hit_box.size.y * 0.5f) ;
            int32 bottom_y = (int32) glm::ceil(transform.position.y - hit_box.size.y * 0.5f);

            bool did_collision = false;
            for (int32 y = top_y; y >= bottom_y; y--)
                for  (int32 x = left_x; x <= right_x; x++)
                    for (int32 z = back_z; z <= front_z; z++)
                    {
                        glm::vec3 box_pos = glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f);
                        Block block = ChunkManager::getBlock(box_pos);
                        BlockFormat block_format = get_block(block.block_id);

                        HitBox default_block_box;
                        default_block_box.size = glm::vec3(1.0f, 1.0f, 1.0f);
                        Transform block_transform;
                        block_transform.front = glm::vec3(1, 0, 0);
                        block_transform.up = glm::vec3(0, 1, 0);
                        block_transform.right = glm::vec3(0, 0, 1);
                        block_transform.orientation = glm::vec3(0, 0, 0);
                        block_transform.scale = glm::vec3(1, 1, 1);
                        block_transform.position = box_pos;

                        if (block_format.m_is_solid && IsColliding(hit_box, transform, default_block_box, block_transform))
                        {

                        }
                    }

        }

        static bool IsColliding(const HitBox &hb1, const Transform &tr1
                , const HitBox &hb2, const Transform &tr2)
        {
            glm::vec3 test_axes[3];
            test_axes[0] = glm::vec3(1, 0, 0);
            test_axes[1] = glm::vec3(0, 1, 0);
            test_axes[2] = glm::vec3(0, 0, 1);

            // find the min overlapping amount
            float min_overlapping = FLT_MAX;


        }

    }

}