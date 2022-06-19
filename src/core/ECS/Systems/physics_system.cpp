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
#include "renderer/renderer.h"


namespace SymoCraft
{
    struct Interval
    {
        float _min;
        float _max;
    };


    /* TODO: The Physics system does not perfect enough. It should be rebuilt afterwards.
    */
    namespace Physics
    {
        // The main component of our physics system is collision
        // The remaining part of it is to deal with collision
        enum class Direction : uint8
        {
            NONE = 0,
            TOP,
            BOTTOM,
            FRONT,
            BACK,
            LEFT,
            RIGHT
        };

        bool compare(float x, float y, float epsilon = std::numeric_limits<float>::min())
        {
            return abs(x - y) <= epsilon * std::max(1.0f, std::max(abs(x), abs(y)));
        }

        bool compare(const glm::vec3& vec1, const glm::vec3& vec2, float epsilon = std::numeric_limits<float>::min())
        {
            return compare(vec1.x, vec2.x, epsilon) && compare(vec1.y, vec2.y, epsilon) && compare(vec1.z, vec2.z, epsilon);
        }

        struct CollisionInfo
        {
            glm::vec3 overlap_part;     // overlap part of two entity
            Direction force;            // collision force direction
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
                                                        , const HitBox &hb2, const Transform& tr2);
        static bool IsColliding(const HitBox &hb1, const Transform &tr1
                                , const HitBox &hb2, const Transform &tr2);

        static Interval GetInterval(const HitBox &box, const Transform &transform, const glm::vec3 &axis);
        static void GetQuadrantResult(const Transform &tr1, const Transform &tr2, const HitBox &hb2_expanded
                                      , CollisionInfo* res ,Direction x_face, Direction y_face, Direction z_face);
        static glm::vec3 ClosetPointOnRay(const glm::vec3 ray_origin, const glm::vec3 ray_direction
                                          , float ray_max_distance, glm::vec3 &point);


        // ----------------------------------------------------------------------------------------------------------
        // Physics system core

        void Update(ECS::Registry& registry)
        {
            static float accumulated_delta_time = 0.0f;
            accumulated_delta_time += Application::delta_time;

            while (accumulated_delta_time >= kPhysicsUpdateRate)
            {
                accumulated_delta_time -= kPhysicsUpdateRate;

                for (ECS::EntityId entity : registry.View<Transform, RigidBody, HitBox>())
                {

                    RigidBody& rb = registry.GetComponent<RigidBody>(entity);
                    Transform& transform = registry.GetComponent<Transform>(entity);
                    HitBox& hit_box = registry.GetComponent<HitBox>(entity);

                    transform.position += rb.velocity * kPhysicsUpdateRate;
                    rb.velocity += rb.acceleration * kPhysicsUpdateRate;
                    if (rb.use_gravity)
                    {
                        rb.velocity -= uniform_gravity * kPhysicsUpdateRate;
                    }
                    rb.velocity = glm::clamp(rb.velocity, -terminal_velocity, terminal_velocity);

                    if (rb.is_sensor)
                    {
                        // not designed yet
                        continue;
                    }

                    ResolveStaticCollision(entity, rb, transform, hit_box);

                }
            }
        }

        // TODO: RayCast implementation
/*
        static bool DoRaycast(const glm::vec3& origin, const glm::vec3& normal_direction,
                              float max_distance, bool draw,
                              const glm::vec3& block_center, const glm::vec3& step, RaycastStaticResult* out);
        // Raycast for player on the block
        RaycastStaticResult RaycastStatic(const glm::vec3 &origin, const glm::vec3 &normal_direction
                , float max_distance, bool draw)
        {
            RaycastStaticResult result;
            result.hit = false;

            if (compare(normal_direction, glm::vec3(0.0, 0.0, 0.0)))
                return result;

            if (draw)
            {
                // TODO: draw a line here
                // from origin to origin + normal_direction * max_distance
            }
        }
*/
        // ----------------------------------------------------------------------------------------------------------
        // Functions Implementation

        // Resolve static collision
        // Parameters: entity id, rigid body, transform, hit box
        static void ResolveStaticCollision(ECS::EntityId entity, RigidBody &rb, Transform &transform, HitBox &hit_box)
        {
            // Get all face coordinate of the hit box
            auto right_x = (int32) glm::ceil(transform.position.x + hit_box.size.x * 0.5f);
            auto left_x = (int32) glm::ceil(transform.position.x - hit_box.size.x * 0.5f);
            auto front_z = (int32) glm::ceil(transform.position.z + hit_box.size.z * 0.5f);
            auto back_z = (int32) glm::ceil(transform.position.z - hit_box.size.z * 0.5f);
            auto top_y = (int32) glm::ceil(transform.position.y + hit_box.size.y * 0.5f) ;
            auto bottom_y = (int32) glm::ceil(transform.position.y - hit_box.size.y * 0.5f);

            HitBox default_block_box{};
            default_block_box.size = glm::vec3(1.0f, 1.0f, 1.0f);
            Transform block_transform{};
            block_transform.front = glm::vec3(1, 0, 0);
            block_transform.up = glm::vec3(0, 1, 0);
            block_transform.right = glm::vec3(0, 0, 1);
            block_transform.yaw = 0;
            block_transform.pitch = 0;
            block_transform.scale = glm::vec3(1, 1, 1);

            bool did_collision = false;
            for (int32 y = top_y; y >= bottom_y; y--)
                for  (int32 x = left_x; x <= right_x; x++)
                    for (int32 z = back_z; z <= front_z; z++)
                    {
                        glm::vec3 box_pos = glm::vec3(x - 0.5f, y - 0.5f, z - 0.5f);
                        Block block = ChunkManager::GetBlock(box_pos);
                        BlockFormat block_format = get_block(block.block_id);

                        block_transform.position = box_pos;

                        if (block_format.m_is_solid && IsColliding(hit_box, transform, default_block_box, block_transform))
                        {
                            CollisionInfo collision_info
                            = StaticCollisionInformation(rb, hit_box, transform, default_block_box, block_transform);

                            float dot_product = glm::dot(glm::normalize(collision_info.overlap_part), glm::normalize(rb.velocity));
                            if (dot_product < 0)
                            {
                                // We're already moving out of the collision, don't do anything
                                continue;
                            }
                            transform.position -= collision_info.overlap_part;
                            switch (collision_info.force)
                            {
                                case Direction::BOTTOM:
                                case Direction::TOP:
                                    rb.acceleration.y = 0;
                                    rb.velocity.y = 0;
                                    break;
                                case Direction::RIGHT:
                                case Direction::LEFT:
                                    rb.velocity.x = 0;
                                    rb.acceleration.x = 0;
                                    break;
                                case Direction::FRONT:
                                case Direction::BACK:
                                    rb.velocity.z = 0;
                                    rb.acceleration.z = 0;
                                    break;
                                default:
                                    break;
                            }
                            rb.on_ground = rb.on_ground || collision_info.force == Direction::BOTTOM;
                            did_collision = true;
                        }
                    }

            if (!did_collision && rb.on_ground && rb.velocity.y > 0)
            {
                // If we're not colliding with any object it's impossible to be on the ground
                rb.on_ground = false;
            }
        }

        static bool IsColliding(const HitBox &hb1, const Transform &tr1
                , const HitBox &hb2, const Transform &tr2)
        {
            glm::vec3 test_axes[3] = {
                    glm::vec3(1, 0, 0),
                    glm::vec3(0, 1, 0),
                    glm::vec3(0, 0, 1) };

            // negative axis face's position
            glm::vec3 hb1_negative = tr1.position - (hb1.size * 0.5f);
            glm::vec3 hb2_negative = tr2.position - (hb2.size * 0.5f);

            // positive axis face's position
            glm::vec3 hb1_positive = tr1.position + (hb1.size * 0.5f);
            glm::vec3 hb2_positive = tr2.position + (hb2.size * 0.5f);

            // check each face if there is an overlapping
            for (int i = 0; i < 3; i++)
            {
                float f1_positive = glm::dot(hb1_positive, test_axes[i]);
                float f2_negative = glm::dot(hb2_negative, test_axes[i]);
                if (f2_negative - f1_positive <= 0.001f)
                    return true;

                float f1_negative = glm::dot(hb1_negative, test_axes[i]);
                float f2_positive = glm::dot(hb2_positive, test_axes[i]);
                if (f1_negative - f2_positive <= 0.001f)
                    return true;
            }
            return false;
        }

        static CollisionInfo StaticCollisionInformation(const RigidBody& rb1, const HitBox &hb1, const Transform& tr1
                , const HitBox &hb2, const Transform& tr2)
        {
            CollisionInfo res;
            res.did_collision = true;
            HitBox hb2_expanded = hb2;
            hb2_expanded.size += hb1.size;

            // Figure out which quadrant the collision is and resolve it
            glm::vec3 hb1_to_hb2 = tr1.position - tr2.position;

            if (hb1_to_hb2.x > 0 && hb1_to_hb2.y > 0 && hb1_to_hb2.z > 0)
            {
                // We are in the top-right-front quadrant
                GetQuadrantResult(tr1, tr2, hb2_expanded, &res, Direction::LEFT, Direction::BOTTOM, Direction::BACK);
            }
            else if (hb1_to_hb2.x > 0 && hb1_to_hb2.y > 0 && hb1_to_hb2.z <= 0)
            {
                // We are in the top-right-back quadrant
                GetQuadrantResult(tr1, tr2, hb2_expanded, &res, Direction::LEFT, Direction::BOTTOM, Direction::FRONT);
            }
            else if (hb1_to_hb2.x > 0 && hb1_to_hb2.y <= 0 && hb1_to_hb2.z > 0)
            {
                // We are in the bottom-right-front quadrant
                GetQuadrantResult(tr1, tr2, hb2_expanded, &res, Direction::LEFT, Direction::TOP, Direction::BACK);
            }
            else if (hb1_to_hb2.x > 0 && hb1_to_hb2.y <= 0 && hb1_to_hb2.z <= 0)
            {
                // We are in the bottom-right-back quadrant
                GetQuadrantResult(tr1, tr2, hb2_expanded, &res, Direction::LEFT, Direction::TOP, Direction::FRONT);
            }
            else if (hb1_to_hb2.x <= 0 && hb1_to_hb2.y > 0 && hb1_to_hb2.z > 0)
            {
                // We are in the top-left-front quadrant
                GetQuadrantResult(tr1, tr2, hb2_expanded, &res, Direction::RIGHT, Direction::BOTTOM, Direction::BACK);
            }
            else if (hb1_to_hb2.x <= 0 && hb1_to_hb2.y > 0 && hb1_to_hb2.z <= 0)
            {
                // We are in the top-left-back quadrant
                GetQuadrantResult(tr1, tr2, hb2_expanded, &res, Direction::RIGHT, Direction::BOTTOM, Direction::FRONT);
            }
            else if (hb1_to_hb2.x <= 0 && hb1_to_hb2.y <= 0 && hb1_to_hb2.z > 0)
            {
                // We are in the bottom-left-front quadrant
                GetQuadrantResult(tr1, tr2, hb2_expanded, &res, Direction::RIGHT, Direction::TOP, Direction::BACK);
            }
            else if (hb1_to_hb2.x <= 0 && hb1_to_hb2.y <= 0 && hb1_to_hb2.z <= 0)
            {
                // We are in the bottom-left-back quadrant
                GetQuadrantResult(tr1, tr2, hb2_expanded, &res, Direction::RIGHT, Direction::TOP, Direction::FRONT);
            }
            else
            {
                AmoLogger_Error("Could not evaluate physics calculation.");
                res.did_collision = false;
            }
            return res;
        }

        static float GetDirection(Direction face)
        {
            switch (face)
            {
                case Direction::BACK:
                    return 1;
                case Direction::FRONT:
                    return -1;
                case Direction::RIGHT:
                    return -1;
                case Direction::LEFT:
                    return 1;
                case Direction::TOP:
                    return -1;
                case Direction::BOTTOM:
                    return 1;
            }

            AmoLogger_Error("Could not get direction from face!");
            return 0.0001f;
        }

        static void GetQuadrantResult(const Transform &tr1, const Transform &tr2, const HitBox &hb2_expanded
                , CollisionInfo* res ,Direction x_face, Direction y_face, Direction z_face)
        {
            float x_direction = GetDirection(x_face);
            float y_direction = GetDirection(y_face);
            float z_direction = GetDirection(z_face);

            glm::vec3 hb2_expanded_size_by_direction = {
                    hb2_expanded.size.x * x_direction,
                    hb2_expanded.size.y * y_direction,
                    hb2_expanded.size.z * z_direction
            };
            glm::vec3 quadrant = (hb2_expanded_size_by_direction * 0.5f) + tr2.position;
            glm::vec3 delta = tr1.position - quadrant;
            glm::vec3 abs_delta = glm::abs(delta);

            if (abs_delta.x < abs_delta.y && abs_delta.x < abs_delta.z)
            {
                // We are colliding with the right-face
                res->overlap_part = delta.x * glm::vec3(1, 0, 0);
                res->force = x_face;
            }
            else if (abs_delta.y < abs_delta.x && abs_delta.y < abs_delta.z)
            {
                // We are colliding with the top-face
                res->overlap_part = delta.y * glm::vec3(0, 1, 0);
                res->force = y_face;
            }
            else
            {
                // We are colliding with the front-face
                res->overlap_part = delta.z * glm::vec3(0, 0, 1);
                res->force = z_face;
            }
        }

        static Interval GetInterval(const HitBox& box, const Transform& transform, const glm::vec3& axis)
        {
            glm::vec3 vertices[8];
            glm::vec3 center = transform.position;
            glm::vec3 half_size = box.size * 0.5f;
            vertices[0] = center + glm::vec3(0, 0, 1) * half_size.z + glm::vec3(0, 1, 0) * half_size.y + glm::vec3(1, 0, 0) * half_size.x; // ForwardTopRight
            vertices[1] = center + glm::vec3(0, 0, 1) * half_size.z + glm::vec3(0, 1, 0) * half_size.y - glm::vec3(1, 0, 0) * half_size.x; // ForwardTopLeft
            vertices[2] = center + glm::vec3(0, 0, 1) * half_size.z - glm::vec3(0, 1, 0) * half_size.y + glm::vec3(1, 0, 0) * half_size.x; // ForwardBottomRight
            vertices[3] = center + glm::vec3(0, 0, 1) * half_size.z - glm::vec3(0, 1, 0) * half_size.y - glm::vec3(1, 0, 0) * half_size.x; // ForwardBottomLeft
            vertices[4] = center - glm::vec3(0, 0, 1) * half_size.z + glm::vec3(0, 1, 0) * half_size.y + glm::vec3(1, 0, 0) * half_size.x; // BackTopRight
            vertices[5] = center - glm::vec3(0, 0, 1) * half_size.z + glm::vec3(0, 1, 0) * half_size.y - glm::vec3(1, 0, 0) * half_size.x; // BackTopLeft
            vertices[6] = center - glm::vec3(0, 0, 1) * half_size.z - glm::vec3(0, 1, 0) * half_size.y + glm::vec3(1, 0, 0) * half_size.x; // BackBottomRight
            vertices[7] = center - glm::vec3(0, 0, 1) * half_size.z - glm::vec3(0, 1, 0) * half_size.y - glm::vec3(1, 0, 0) * half_size.x; // BackBottomLeft

            Interval result;
            result._min = result._max = glm::dot(axis, vertices[0]);
            for (int i = 1; i < 8; i++)
            {
                float projection = glm::dot(axis, vertices[i]);
                result._min = glm::min(result._min, projection);
                result._max = glm::max(result._max, projection);
            }

            return result;
        }

        static glm::vec3 ClosetPointOnRay(const glm::vec3 ray_origin, const glm::vec3 ray_direction
                , float ray_max_distance, glm::vec3 &point)
        {
            glm::vec3 origin_to_point = point - ray_origin;
            glm::vec3 ray_segment = ray_direction * ray_max_distance;

            float rayMagnitudeSquared = ray_max_distance * ray_max_distance;
            float dot_product = glm::dot(origin_to_point, ray_segment);
            float distance = dot_product / rayMagnitudeSquared; //The normalized "distance" from a(point) to your closest point

            if (distance < 0)     //Check if P projection is over vectorAB
            {
                return ray_origin;
            }
            else if (distance > 1)
            {
                return ray_origin + (ray_direction * ray_max_distance);
            }

            return ray_origin + ray_direction * distance * ray_max_distance;
        }


    }

}