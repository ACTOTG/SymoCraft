//
// Created by Amo on 2022/6/15.
//

#ifndef SYMOCRAFT_APPLICATION_H
#define SYMOCRAFT_APPLICATION_H

namespace SymoCraft
{
    struct Window;
    //struct FrameBuffer;
    class GlobalThreadPool;

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

        // Delta time
        extern float delta_time;
    }
}

#endif //SYMOCRAFT_APPLICATION_H
