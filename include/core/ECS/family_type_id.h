//
// Created by Amo on 2022/6/16.
//

#ifndef ENTITY_COMPONENT_SYSTEM_H_FAMILY_TYPE_ID_H
#define ENTITY_COMPONENT_SYSTEM_H_FAMILY_TYPE_ID_H
#include "core.h"

namespace SymoCraft::EntityComponentSystem::Internal
        {
            // TypeId should be uint32_t
            using TypeId = uint32;

            // @Amo
            // Class name: Family Type ID
            // Brief: Entity or Component or System can use it to get its unique TypeID
            template<class T>
            class FamilyTypeId
            {
            public:
                template<class U>
                static const TypeId GetTypeId()
                {
                    static const TypeId kStaticTypeId = s_counter++;
                    return kStaticTypeId;
                }

                static const TypeId GetNum()
                {
                    return s_counter;
                }

            private:
                static TypeId s_counter;
            };
        }

#endif //ENTITY_COMPONENT_SYSTEM_H_FAMILY_TYPE_ID_H
