//
// Created by Amo on 2022/6/15.
//

#define STB_IMAGE_IMPLEMENTATION
#include "Core/application.h"
#include "core.h"
#include "Core/window.h"
#include "Core/global_thread_pool.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"

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
            TextureArray texture_array;
            texture_array = texture_array.CreateAtlasSlice("assets/textures/texture.png", true);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

            // -------------------------------------------------------------------
            // Render Loop
            while (!window.ShouldClose())
            {
                double current_frame_time = glfwGetTime();
                delta_time = (float)(current_frame_time - previous_frame_time);

                glClear(GL_COLOR_BUFFER_BIT);


                //Add blocks, play as you want
                for( float i = 0.0f; i < 6; )
                {
                    for( float j = 0.0f; j < 6;)
                    {
                        Renderer::AddBlocksToBatch( glm::vec3(i, 0.0f, j), 16, 32, 0);
                        j += 1.0f;
                    }
                    i += 1.0f;
                }

                SymoCraft::Renderer::ReportStatus();
                glEnable(GL_DEPTH_TEST);
                glBindTextureUnit(0, texture_array.m_texture_Id);
                SymoCraft::Renderer::ClearBuffers();
                SymoCraft::Renderer::Render();


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

        GlobalThreadPool& GetGlobalThreadPool()
        {
            return *global_thread_pool;
        }

    }
}
