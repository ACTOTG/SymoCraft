//
// Created by Amo on 2022/6/15.
//

#ifndef SYMOCRAFT_APPLICATION_H
#define SYMOCRAFT_APPLICATION_H


struct GLFWwindow;
namespace SymoCraft
{
    struct Window;
    //struct FrameBuffer;
    class GlobalThreadPool;
    class Camera;


    namespace Application
    {
        // Initializing application
        void Init();

        // Run application
        void Run();

        // Free application
        void Free();

        // Get Window Reference
        Window& GetWindow();
        //
        // Get Global Thread Pool Reference
        GlobalThreadPool& GetGlobalThreadPool();

        // Get Camera Pointer
        Camera* GetCamera();

        // Need to be replaced in the following development
        void processInput(GLFWwindow* window);
        void MouseMovementCallBack(GLFWwindow* window, double x_pos_in, double y_pos_in);

        // Delta time
        extern float delta_time;
    }
}

#endif //SYMOCRAFT_APPLICATION_H
