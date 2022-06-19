#include <MemoryAllocator/AmoBase.h>
#include "core/application.h"
#include "core/ECS/registry.h"
#include "core/ECS/component.h"
#include "core/global_thread_pool.h"

int main()
{
#ifdef _DEBUG
    AmoBase::AmoMemory_Init(true, 1024);
#endif
    SymoCraft::Application::Init();
    SymoCraft::Application::Run();
    SymoCraft::Application::Free();

    AmoBase::AmoMemory_MemoryLeaksDetected();
}