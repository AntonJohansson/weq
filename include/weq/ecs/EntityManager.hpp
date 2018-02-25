#pragma once

#include <weq/utility/NumberTypes.hpp>
#include <weq/memory/PoolAllocator.hpp>
#include <weq/ecs/Component.hpp>

#include <functional>
#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include <bitset>

// Temp
#include <iostream>

// @TODO in order to dealloc components, a ptr to the component
// is stored in a map from (id <-> void*). This is suboptimal,
// perhaps a better allocator can be used instead of a pool map;
// An allocator with [id] -> ptr support would be nice, but requries
// fixed pointer locations.

namespace weq{

using namespace memory;

// max components = 64
using ComponentMask = std::bitset<64>;
using EntityId = u64;
using ComponentId = u64;



class EntityManager{
public:
  EntityId create(){
    static EntityId entity_counter = 0;

    EntityId id;
    if(_free_ids.empty()){
      id = entity_counter++;
    }else{
      id = _free_ids.front();
      _free_ids.pop();
    }

    return id;
  }

  void destroy(EntityId id){
    _entity_component_masks[id].reset();
    _free_ids.push(id);
  }

  template<typename C, typename... Args>
  C* assign(EntityId entity_id, Args&&... args){
    // Get component id
    ComponentId component_id = ComponentTypeId<C>::id();

    // Get pool and allocate component
    PoolAllocator<C>* pool = component_pool<C>();
    C* component = pool->alloc(std::forward<Args>(args)...);


    // Set bit mask for entity
    _entity_component_masks[entity_id].set(component_id);

    // @TODO currently documenting component ptr, should I do it this way?
    _component_ptrs[entity_id][component_id] = reinterpret_cast<void*>(component);

    return component;
  }

  // @TODO check if component actually exists :)
  template<typename C>
  C* get(EntityId entity_id){
    // Get component id
    ComponentId component_id = ComponentTypeId<C>::id();
    // Get ptr
    C* ptr = reinterpret_cast<C*>(_component_ptrs[entity_id][component_id]);

    return ptr;
  }

  template<typename C>
  void remove(EntityId entity_id){
    ComponentId component_id = ComponentTypeId<C>::id();
    // Reset component mask for component
    _entity_component_masks[entity_id].reset(component_id);

    // Get component btr
    // @TODO check if a pointer is present for component_id,
    // instead of assuming it is
    C* ptr = reinterpret_cast<C*>(_component_ptrs[entity_id][component_id]);

    // Get pool
    auto pool = component_pool<C>();

    // Dealloc ptr in pool
    pool->dealloc(ptr);
  }

  std::vector<EntityId> entities_with_components(ComponentMask& match_mask){
    std::vector<EntityId> entities;
    for(auto [id, mask]: _entity_component_masks){
      if((match_mask & mask) == match_mask){
        entities.push_back(id);
      }
    }
    return entities;
  }

  template<typename... Components>
  std::vector<EntityId> entities_with_components(){
    auto mask = generate_component_mask<Components...>();
    return entities_with_components(mask);
  }

  // @TODO WHYY is this required??
  template<typename T>
  struct Identity{using type = T;};

  template<typename... Components>
  void each(typename Identity<std::function<void(EntityId, Components&...)>>::type func){
    auto entities = entities_with_components<Components...>();
    for(auto entity : entities){
      func(entity, *get<Components>(entity)...);
    }
  }

  template<typename C>
  bool has_component(EntityId entity_id){
    ComponentId component_id = ComponentTypeId<C>::id();
    return _entity_component_masks[entity_id].test(component_id);
  }

  // Functions for generating a component mask for arbitrary components
  //@TODO cache already used component_masks
  template<typename... Components>
  ComponentMask generate_component_mask(){
    ComponentMask mask;
    (mask.set(ComponentTypeId<Components>::id()), ...);
    return mask;
  }

  // Returns string of entities and components
  std::string debug_info(){
    std::string str = "Entities (id)\n";
    each<>([&str](EntityId id){
        str += std::to_string(id) + "\n";
      });

    return str;
  }

  // Returns the number of entities
  u64 size(){
    return _entity_component_masks.size();
  }

private:
  inline void assert_valid(EntityId id){
    //assert(id < _entity_component_masks.find(id)
  }

  template<typename C>
  PoolAllocator<C>* component_pool(){
    ComponentId component_id = ComponentTypeId<C>::id();

    auto it = _component_pools.find(component_id);
    if(it == _component_pools.end()){
      PoolAllocator<C>* pool = new PoolAllocator<C>(8, RESIZE);
      _component_pools[component_id] = static_cast<Allocator*>(pool);
    }

    return static_cast<PoolAllocator<C>*>(_component_pools[component_id]);
  }

  // Member variables
  std::unordered_map<EntityId, std::unordered_map<ComponentId, void*>>      _component_ptrs;
  std::unordered_map<EntityId, ComponentMask> _entity_component_masks;
  std::unordered_map<ComponentId, Allocator*> _component_pools;
  std::queue<u64> _free_ids;
};

}
