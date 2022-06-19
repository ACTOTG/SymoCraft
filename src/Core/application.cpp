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

namespace SymoCraft
{

    namespace Application
    {
        float delta_time = 0.016f;

        // Internal variables
        static GlobalThreadPool* global_thread_pool;


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
            Renderer::Init();
        }

        void Run()
        {
            Window& window = GetWindow();
            double previous_frame_time = glfwGetTime();

            stbi_set_flip_vertically_on_load(true);
            TextureArray texture_array;
            texture_array = texture_array.CreateAtlasSlice("../assets/textures/texture_atlas.png", true);

            AmoLogger_Log("Temporary CAMERA class is waiting to be replaced");
            AmoLogger_Warning("Temporary Input Process Function is waiting to be replaced");
            // need to be replaced

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
                    ChunkManager::getAllChunks().find({x,z})->second.generateRenderData();

            block_batch.ReloadData();
            Report();
            // -------------------------------------------------------------------
            // Render Loop
            while (!window.ShouldClose())
            {
                double current_frame_time = glfwGetTime();
                delta_time = (float)(current_frame_time - previous_frame_time);

                // Temporary Input Process Function
                processInput((GLFWwindow*)GetWindow().window_ptr);

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

        GlobalThreadPool& GetGlobalThreadPool()
        {
            return *global_thread_pool;
        }

        void MouseMovementCallBack(GLFWwindow* window, double x_pos_in, double y_pos_in)
        {
            GetCamera()->InsMouseMovementCallBack(window, x_pos_in, y_pos_in);
        }

        void processInput(GLFWwindow* window)
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

            // --------------------------------------------------------------------------------------------
            // process input for camera moving
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
        }
    }

}
