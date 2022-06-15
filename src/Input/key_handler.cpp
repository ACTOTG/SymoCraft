//
// Created by Amo on 2022/6/15.
//

#include "Input/key_handler.h"
#include "Input/input.h"
#include "Input/key_bindings.h"
#include "Core/application.h"
#include "Core/window.h"

namespace SymoCraft
{
    namespace KeyHandler
    {
        void Update()
        {
            if (KeyBindings::IsKeyBeginPressed(KeyBind::Exit))
            {
                Application::GetWindow().Close();
            }
        }
    }
}