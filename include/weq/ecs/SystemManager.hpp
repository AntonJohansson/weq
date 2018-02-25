#pragma once

#include <weq/ecs/System.hpp>
#include <weq/utility/NumberTypes.hpp>
#include <weq/memory/CircularList.hpp>
#include <unordered_map>
#include <memory>

namespace weq{

class SystemManager{
public:
  using SystemId = u64;

  // @TODO check that S::id() is valid
  template<typename S, u8 prio = 0,typename... Args>
  S* add(Args&&... args){
    S* system = new S(std::forward<Args>(args)...);
    BaseSystem* base = static_cast<BaseSystem*>(system);

    _systems[S::id()] = base;
    _system_order.add(base, prio);

    return system;
  }

  void update_all(Managers& managers, f32 dt){
    _system_order.for_each([&managers, &dt](auto node){
        node->data->update(managers, dt);
      });
  }

  void configure(Managers& managers){
    _system_order.for_each([&managers](auto node){
        node->data->configure(managers);
      });
  }

  void reset(){
    _systems.clear();
    _system_order.reset();
  }

  u64 size(){
    return _systems.size();
  }

private:
  std::unordered_map<SystemId, BaseSystem*> _systems;
  memory::CircularList<BaseSystem*> _system_order;
};

}
