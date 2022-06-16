//
// Created by Amo on 2022/6/16.
//

#include "core/ECS/family_type_id.h"

namespace SymoCraft::EntityComponentSystem
    {
        class IEntity;
        class IComponent;
        class ISystem;

        namespace Internal
        {
            template<> TypeId FamilyTypeId<IEntity>::s_counter = 0;
            template<> TypeId FamilyTypeId<IComponent>::s_counter = 0;
            // template<> TypeId FamilyTypeId<ISystem>::s_counter = 0;

            template class FamilyTypeId<IEntity>;
            template class FamilyTypeId<IComponent>;
            // template class FamilyTypeId<ISystem>;

        }
    }