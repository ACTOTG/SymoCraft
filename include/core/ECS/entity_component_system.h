//
// Created by Amo on 2022/6/16.
//

#ifndef SYMOCRAFT_ENTITY_COMPONENT_SYSTEM_H
#define SYMOCRAFT_ENTITY_COMPONENT_SYSTEM_H
#include "core.h"

namespace SymoCraft
{
    namespace EntityComponentSystem
    {
        template<typename ...Components>
        class RegistryView;

        template<typename T>
        int32 ComponentId();

        using EntityId = uint32;

        extern EntityId null_entity; // null_entity = Internal::CreateEntityId(UINT32_MAX, 0);

        namespace Internal
        {
            extern int32 component_counter;
            const int spares_set_pool_size = 8;
            const int32 max_num_components = 256;

            // Is Entity ID Valid
            // return true when entity id is not null_entity
            inline bool IsEntityIdValid(EntityId _id)
            {
                return _id != null_entity;
            }

            struct SpareSetPool
            {
                EntityId start_id;
                EntityId entities[spares_set_pool_size];

                // Initializing a spare set pool
                void Init()
                {
                    for (int i = 0; i < spares_set_pool_size; i++)
                        entities[i] = null_entity;
                }
            };

            struct SpareSet
            {
                int component_id;               // component id
                uint32 max_num_components;      // the max number of components
                uint32 num_components;          // the current number of components
            };

        }

        struct Registry
        {
            std::vector<EntityId> entities;
        };

        template<typename ...Components>
        class RegistryView
        {
        public:
            // Constructor
            // Parameters:

        };


    }
}



#endif //SYMOCRAFT_ENTITY_COMPONENT_SYSTEM_H
