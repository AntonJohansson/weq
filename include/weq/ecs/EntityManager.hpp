#pragma once

#include <weq/utility/IntegerTypes.hpp>
#include <weq/memory/PoolAllocator.hpp>
#include <weq/ecs/Component.hpp>
#include <queue>
#include <unordered_map>
#include <bitset>

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

  template<typename C, typename... Args>
  C* assign(EntityId entity_id, Args&&... args){
    // Get component id
    ComponentId component_id = ComponentTypeId<C>::id();

    // Get pool and allocate component
    PoolAllocator<C>* pool = component_pool<C>();
    C* component = pool->alloc(std::forward<Args>(args)...);

    // Set bit mask for entity
    _entity_component_masks[entity_id].set(component_id);

    return component;
  }

  void entities_with_components(ComponentMask& match_mask){
    for(auto [id, mask] pair : _entity_component_masks){
      if(mask == match_mask){
      }
    }
  }

  template<typename... Components>
  void entities_with_components(){
    auto mask = generate_component_mask<Components...>();
    entities_with_components(mask);
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


  //// Base case
  //template<typename C>
  //ComponentMask generate_component_mask(){
  //  ComponentMask mask;
  //  mask.set(ComponenTypeId<C>::id());
  //  return mask;
  //}

  //// "Peel" off one component and or it with the remaining ones
  //template<typename C1, typname C2, typename... Components>
  //ComponentMask generate_component_mask(){
  //  return generate_component_mask<C1>() | generate_component_mask<C2, Components...>
  //}

  // Member variables
  std::unordered_map<EntityId, ComponentMask> _entity_component_masks;
  std::unordered_map<ComponentId, Allocator*> _component_pools;
  std::queue<u64> _free_ids;
};

}
