//
// Created by Amo on 2022/6/17.
//

#ifndef SYMOCRAFT_REGISTRY_H
#define SYMOCRAFT_REGISTRY_H
#include "core.h"
#include "component_container.h"


namespace SymoCraft
{
    //enum class TagType : uint8;

    namespace ECS
    {
        typedef uint32 EntityIndex;
        typedef uint32 EntityVersion;
        typedef uint64 EntityId;
        typedef uint32 ComponentIndex;

        class Registry
        {
        public:
            // -------------------------------------------------------------------
            // Registry Functions

            // Free
            void Free();

            // Clear
            // Brief: clear all data member
            void Clear();

            // -------------------------------------------------------------------
            // Entity Registry

            // Create an entity
            // return a new entity id
            EntityId CreateEntity();

            // The component number of entity
            // Parameters: entity id
            int NumComponents(EntityId entity) const;

            // Check if the entity is valid
            // Parameters: Entity id
            bool IsEntityValid(EntityId entity) const;

            // Destroy an entity
            // Parameters: entity id
            void DestroyEntity(EntityId entity);

            // Find an entity
            // Return an entity id
            // Parameters: TagType
            //EntityId Find(TagType type);

            // -------------------------------------------------------------------
            // Component Registry

            // Register a component
            // Parameters: debug name
            template<typename T>
            void RegisterComponent(const char *debug_name)
            {
                int32 com_type = ECS::Internal::GetComponentType<T>();
                EntityIndex index = 0;
                AmoLogger_Notice(com_type == component_set.size(), "Tried to register component '%s' twice", debug_name);
                component_set.template emplace_back(Internal::ComponentContainer::DefaultComponentContainer<T>(index));
                AmoLogger_Notice(com_type < Internal::kMaxNumComponents, "Exceeded the maximum number of components.");
                debug_component_names.template emplace_back(std::string(debug_name));
            }

            // Add a Component to an entity
            // return a component reference
            // Parameters: entity id
            template<typename T>
            T& AddComponent(EntityId entity)
            {
                int32 component_type = ECS::Internal::GetComponentType<T>();
                const EntityIndex index = Internal::GetEntityIndex(entity);

                AmoLogger_Notice(component_type < component_set.size(),
                                 "You need to register all components in the same order *everywhere*. "
                                 "Component '%s' was not registered.", typeid(T).name());

                // Get or Add the component at this index
                T& component = *component_set[component_type].template AddOrGet<T>(entity);
                return component;
            }

            // Has Component by Component Type?
            // Parameters: entity id, component type
            bool HasComponentByType(EntityId entity, int32 component_type) const;

            // Has Component?
            // Parameters: entity id
            template<typename T>
            bool HasComponent(EntityId entity) const
            {
                return HasComponentByType(entity, Internal::GetComponentType<T>());
            }

            // Get Component By Type
            // return a data pointer
            // Parameters: entity id, component type
            uint8* GetComponentByType(EntityId entity, int32 component_type) const;

            // Get Component
            // return a component reference
            // Parameters: entity id
            template<typename T>
            T& GetComponent(EntityId entity) const
            {
                if (!IsEntityValid(entity))
                {
                    AmoLogger_Error("Cannot check if invalid entity %d has a component."
                    , entity);
                    return nullptr;
                }

                int32 component_type = Internal::GetComponentType<T>();

                if (component_type >= component_set.size() || component_type < 0)
                {
                    AmoLogger_Warning("Tried to check if an entity had component '%d', "
                                      "but a component of type '%d' does not exist in the registry."
                    , component_type, component_type);
                    return nullptr;
                }

                return *component_set[component_type].template Get<T>(Internal::GetEntityIndex(entity));
            }

            // Remove Component
            // Parameters: entity id
            template<typename T>
            void RemoveComponent(EntityId entity)
            {
                if (!IsEntityValid(entity))
                {
                    AmoLogger_Error("Tried to remove invalid entity %d 's component."
                    , entity);
                    return;
                }

                int32 component_type = Internal::GetComponentType<T>();
                EntityIndex entity_index = Internal::GetEntityIndex(entity);
                if (entity_index >= entities.size())
                {
                    AmoLogger_Error("Tried to remove a component from invalid entity '%d'", entity);
                    return;
                }
                if (component_type < 0 || component_type >= component_set.size())
                {
                    AmoLogger_Error("Tried to remove component that does not exist '%d'", component_type);
                    return;
                }

                component_set[component_type].Remove(entity);
            }

            // Remove all component of an entity
            void RemoveAllComponent(EntityId entity);

        private:
            std::vector<EntityId> entities;
            std::vector<Internal::ComponentContainer> component_set;
            std::vector<EntityId> free_entities;
            std::vector<std::string> debug_component_names;
        };
    }
}
#endif //SYMOCRAFT_REGISTRY_H
