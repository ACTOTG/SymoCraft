#include <iostream>
#include <Core.h>



int main() {
    AmoBase::AmoMemory_Init( true);
    Window::Init();
    Window* first_window = Window::Create("Hello World");


    for (;!first_window->ShouldClose();)
    {

    }

    Window::Free();
    return 0;
}
