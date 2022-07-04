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

    // TODO: *Rebuild* the whole entity component system!!
    namespace ECS
    {
        typedef uint32 EntityIndex;
        typedef uint32 EntityVersion;
        typedef uint64 EntityId;
        typedef uint32 ComponentIndex;

        template<typename ...Components>
        class RegistryViewer;
        
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
                }

                int32 component_type = Internal::GetComponentType<T>();

                if (component_type >= component_set.size() || component_type < 0)
                {
                    AmoLogger_Warning("Tried to check if an entity had component '%d', "
                                      "but a component of type '%d' does not exist in the registry."
                    , component_type, component_type);
                }

                AmoLogger_Notice(HasComponent<T>(entity), "Entity '%d' does not have component '%d'"
                                 , entity, component_type);
                // TODO: This will crash if the component is null,
                //  should we return a null component?
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

            // ---------------------------------------------------------------------------------------------------
            // Registry Viewer
            template<typename... Components>
            RegistryViewer<Components...> View()
            {
                return RegistryViewer<Components...>(*this);
            }

            RawMemory Serialize();

        public:
            std::vector<EntityId> entities;

        private:
            std::vector<Internal::ComponentContainer> component_set;
            std::vector<EntityId> free_entities;
            std::vector<std::string> debug_component_names;
        };

        /* ---------------------------------------------------------------------------------------------------
           About registry viewer:
           If we want to search an entity that have several components, the functionalities
           of registry itself are not enough.
           What I want to do is create a viewer that can look through the registry to get this kind of entity.

           The first thing is to store what kinds of components does the entity has?
           The solution is to create a bitset of all the components. set the corresponding component_type bit to 1

           The second problem is how to loop the registry and get what we want?
           The solution is to create an iterator for the registry looping all the entities in it
        */
        class Iterator
        {
        public:
            // constructor
            // Parameters: Registry reference, entity index, components need, is searching for all the component
            Iterator( Registry& reg, EntityIndex index
                       , std::bitset<Internal::kMaxNumComponents>com_need
                       , bool all);

            // Indirect operator = Indexing operator
            // Return an entity index
            EntityIndex operator*() const;

            // identity operator
            bool operator ==(Iterator& other) const;

            bool operator !=(Iterator& other) const;

            // Increment operator
            // skip the unqualified elements
            Iterator& operator++();

        private:
            Registry& registry;
            EntityIndex entity_index;
            std::bitset<Internal::kMaxNumComponents>components_need;
            bool _is_searching_all;

            // Is it a valid index and has the correct component;
            bool IsIndexValid();
        };

        template<typename... Components>
        class RegistryViewer
        {
            friend class Iterator;
        public:
            explicit RegistryViewer(Registry &reg)
            : registry(reg)
            {
                _is_searching_all = sizeof...(Components) == 0;
                if (!_is_searching_all)
                {
                    int component_type[] = {0 , ECS::Internal::GetComponentType<Components>() ...};
                    for (int i = 1; i <= (sizeof...(Components)); i++)
                        components_need.set(component_type[i]);
                }
            }

            const Iterator begin() const
            {
                int first_index = 0;
                for (; first_index < registry.entities.size() &&
                        (
                                !HasRequiredComponents(registry, components_need, registry.entities[first_index]) ||
                                !registry.IsEntityValid(registry.entities[first_index])
                                );)
                    first_index++;
                return Iterator(registry, first_index, components_need, _is_searching_all);
            }

            const Iterator end() const
            {
                return Iterator(registry, (EntityIndex)registry.entities.size(), components_need, _is_searching_all);
            }

        private:
            Registry& registry;
            std::bitset<Internal::kMaxNumComponents> components_need;
            bool _is_searching_all;

            static bool HasRequiredComponents(Registry& reg, const std::bitset<Internal::kMaxNumComponents> &need,
                                             EntityIndex entity_index)
            {
                bool has_required_components = true;
                for (int i = 0; i < need.size(); i++)
                    if (need.test(i) && !reg.HasComponentByType(entity_index, i))
                    {
                        has_required_components = false;
                        break;
                    }
                return has_required_components;
            }
        };
    }
}
#endif //SYMOCRAFT_REGISTRY_H
