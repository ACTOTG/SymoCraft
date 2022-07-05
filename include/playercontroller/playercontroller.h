//
// Created by Amo on 2022/7/5.
//

#ifndef SYMOCRAFT_PLAYERCONTROLLER_H
#define SYMOCRAFT_PLAYERCONTROLLER_H
#include "core.h"

namespace SymoCraft
{
    namespace Application
    {
        extern const float kBlockPlaceDebounceTime;
        extern float block_place_debounce;
        extern uint16 new_block_id;
    }
    namespace ECS
    {
        class Registry;
    }

    class Window;
    namespace PlayerController
    {
        void DoRayCast(ECS::Registry &registry, Window &window);
    }
}

#endif //SYMOCRAFT_PLAYERCONTROLLER_H
