//
// Created by Amo on 2022/6/15.
//

#define STB_IMAGE_IMPLEMENTATION
#include "core/application.h"
#include "core.h"
#include "core/window.h"
#include "core/global_thread_pool.h"
#include "renderer/texture.h"
#include "renderer/renderer.h"
#include "world/chunk.h"
#include "core/ECS/registry.h"
#include "core/ECS/Systems/transform_system.h"
#include "core/ECS/Systems/character_system.h"
#include "core/ECS/Systems/physics_system.h"
#include "core/ECS/component.h"
#include "world/world.h"

namespace SymoCraft
{

    static bool first_enter = true;  // is first enter of cursor? initialized by true


    namespace Application
    {
        float delta_time = 0.016f;

        // Internal variables
        static GlobalThreadPool* global_thread_pool;
        static Camera* camera;

        void Init()
        {
            Window::Init();
            Window& window = GetWindow();   // Get a reference pointer of the only Window
            if (!window.window_ptr)
            {
                AmoLogger_Error("Error: Could not create a window. ");
                return;
            }

            // Initialize all other subsystems.
            // global_thread_pool = new GlobalThreadPool(std::thread::hardware_concurrency());
            ECS::Registry &registry = GetRegistry();
            registry.RegisterComponent<Transform>("Transform");
            registry.RegisterComponent<Physics::RigidBody>("RigidBody");
            registry.RegisterComponent<Physics::HitBox>("HigBox");
            registry.RegisterComponent<Character::CharacterComponent>("CharacterComponent");
            registry.RegisterComponent<Character::PlayerComponent>("PlayerComponent");

            Renderer::Init();
            World::Init();

            camera = GetCamera();
        }

        void Run()
        {

            std::cout << player << std::endl;
            Window& window = GetWindow();
            double previous_frame_time = glfwGetTime();

            stbi_set_flip_vertically_on_load(true);
            TextureArray texture_array;
            texture_array = texture_array.CreateAtlasSlice("../assets/textures/texture_atlas.png", true);

            AmoLogger_Log("Temporary CAMERA class is waiting to be replaced");
            AmoLogger_Warning("Temporary Input Process Function is waiting to be replaced");
            // need to be replaced

            glfwSetScrollCallback( (GLFWwindow *) window.window_ptr, MouseScrollCallBack);
            glfwSetCursorPosCallback((GLFWwindow *) window.window_ptr, MouseMovementCallBack);
            glfwSetInputMode((GLFWwindow*)window.window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // 出生在区块里，开局往天上飞，不然看不见XD
            // Manual chunk generation for testing
            InitializeNoise();
            for(int x = -3; x <= 3; x++)
                for(int z = -3; z <= 3; z++)
                    ChunkManager::queueCreateChunk({x,z});

            for( auto chunk : ChunkManager::getAllChunks() )
                chunk.second.GenerateTerrain();

            ChunkManager::patchChunkPointers();

            // We only need to calculate the vertices once, since we can't remove or add blocks now
            for(int x = -2; x <= 2; x++)
                for(int z = -2; z <= 2; z++)
                    ChunkManager::getAllChunks().find({x, z})->second.GenerateRenderData();

            block_batch.ReloadData();
            Report();

            ECS::Registry &registry = GetRegistry();
            glm::vec3 start_pos{-30.0f, 70.0f, -30.0f};
            Transform &transform = registry.GetComponent<Transform>(World::GetPlayer());
            transform.position = start_pos;
            // -------------------------------------------------------------------
            // Render Loop
            while (!window.ShouldClose())
            {
                double current_frame_time = glfwGetTime();
                delta_time = (float)(current_frame_time - previous_frame_time);

                // Temporary Input Process Function
                processInput((GLFWwindow*)GetWindow().window_ptr);

                //::Registry &registry = GetRegistry();

                TransformSystem::Update(GetRegistry());
                Physics::Update(GetRegistry());
                Character::Player::Update(GetRegistry());

                glBindTextureUnit(0, texture_array.m_texture_Id);
                Renderer::Render();
                Renderer::ReportStatus();

                window.SwapBuffers();
                window.PollInt();

                previous_frame_time = current_frame_time;
            }
        }

        void Free()
        {
            // Free assets

            // Free resources
            global_thread_pool->Free();
            delete global_thread_pool;

            Window& window = GetWindow();
            window.Destroy();
            Window::Free();

        }

        Window& GetWindow()
        {
            static Window* window = Window::Create("SymoCraft");
            return *window;
        }

        Camera* GetCamera()
        {
            static Camera* camera = new Camera(GetWindow().width, GetWindow().height);
            return camera;
        }

        ECS::Registry &GetRegistry()
        {
            static ECS::Registry* registry = new ECS::Registry;
            return *registry;
        }

        GlobalThreadPool& GetGlobalThreadPool()
        {
            return *global_thread_pool;
        }

        void MouseMovementCallBack(GLFWwindow* window, double xpos_in, double ypos_in)
        {
            static float last_x = 0;       // last x position of cursor
            static float last_y = 0;       // last y position of cursor
            ECS::Registry &registry = Application::GetRegistry();
            //Transform &transform = registry.GetComponent<Transform>(camera->entity_id);
            Transform &transform = registry.GetComponent<Transform>(World::GetPlayer());
            Character::PlayerComponent &player_com = registry.GetComponent<Character::PlayerComponent>(World::GetPlayer());
            auto xpos = static_cast<float>(xpos_in);
            auto ypos = static_cast<float>(ypos_in);

            // modify the first enter of the mouse
            if (first_enter)
            {
                last_x = xpos;
                last_y = ypos;
                first_enter = false;
            }
            float xoffset = xpos - last_x;
            float yoffset = last_y - ypos;   // reversed since y-coordinates range from bottom to top
            last_x = xpos;
            last_y = ypos;

            std::cout << xoffset << ' ' << yoffset << std::endl;

            const float sensitivity = 0.05f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;


            transform.pitch += yoffset;
            transform.yaw += xoffset;
            transform.pitch = glm::clamp(transform.pitch, -89.0f, 89.0f);


        }

        void MouseScrollCallBack(GLFWwindow* window, double x_pos_in, double y_pos_in)
        {
            GetCamera()->InsMouseScrollCallBack(window, x_pos_in, y_pos_in);
        }
        void processInput(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            ECS::Registry &registry = GetRegistry();
            Character::CharacterComponent &player_com = registry.GetComponent<Character::CharacterComponent>(World::GetPlayer());

            // --------------------------------------------------------------------------------------------
            // process input for camera moving
            /*
            float camera_displacement = 0;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                camera_displacement = 5.0f * delta_time;        // 5.0 unit per second (actual displacement)
            else
                camera_displacement = 2.5f * delta_time;         // 2.5 unit per second (actual displacement)

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                GetCamera()->CameraMoveBy(FORWARD, camera_displacement);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                GetCamera()->CameraMoveBy(BACKWARD, camera_displacement);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                GetCamera()->CameraMoveBy(RIGHT, camera_displacement);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                GetCamera()->CameraMoveBy(LEFT, camera_displacement);
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                GetCamera()->CameraMoveBy(UPWARD, camera_displacement);
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                GetCamera()->CameraMoveBy(DOWNWARD, camera_displacement);
                */

            player_com.is_running = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;


            player_com.movement_axis.x =
                    glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS
                    ? 1.0f
                    :glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS
                      ? -1.0f
                      : 0.0f;
            player_com.movement_axis.z =
                    glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS
                    ? 1.0f
                    :
                    glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS
                      ? -1.0f
                      : 0.0f;

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                if (!player_com.is_jumping)
                {
                    player_com.apply_jump_force = true;
                }
            }

        }
    }

}
