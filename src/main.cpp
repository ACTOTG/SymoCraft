#include <iostream>
#include <Core.h>
#include "renderer/renderer.h"
#include "renderer/texture.h"


int main() {
    AmoBase::AmoMemory_Init( true);
    Window::Init();
    Window* first_window = Window::Create("Hello World");

    SymoCraft::Renderer::Init();
    TextureArray texture_array;
    texture_array = texture_array.CreateAtlasSlice("assets/textures/texture.png", true);

    // Render Loop
    // =========================================================
    for (;!first_window->ShouldClose();)
    {
        //Add blocks, play as you want
        for( float i = 0.0f; i < 6; )
        {
            for( float j = 0.0f; j < 6;)
            {
                SymoCraft::Renderer::AddBlocksToBatch( glm::vec3(i, 0.0f, j), 16, 32, 0);
                j += 1.0f;
            }
            i += 1.0f;
        }

        SymoCraft::Renderer::ReportStatus();
        glEnable(GL_DEPTH_TEST);
        glBindTextureUnit(0, texture_array.m_texture_Id);
        SymoCraft::Renderer::ClearBuffers();
        SymoCraft::Renderer::Render();

        Window::SwapBuffers();
        Window::PollInt();//???
    }
    // =========================================================

    SymoCraft::Renderer::Free();
    Window::Free();
    return 0;
}
