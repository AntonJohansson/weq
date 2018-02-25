#pragma once

#include <weq/ecs/System.hpp>
#include <weq/utility/NumberTypes.hpp>
#include <weq/memory/CircularList.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace weq{

class SystemManager{
public:
  using SystemId = u64;

  // @TODO check that S::id() is valid
  template<typename S, u8 prio = 255,typename... Args>
  S* add(Args&&... args){
    S* system = new S(std::forward<Args>(args)...);
    BaseSystem* base = static_cast<BaseSystem*>(system);

    _systems[S::id()] = base;
    _system_order.add(base, prio);

    return system;
  }

  void for_each(std::function<void(BaseSystem*)> func){
    _system_order.for_each(func);
  }

  void update_all(EntityManager& entities, EventManager& events, f32 dt){
    _system_order.for_each([&entities, &events, &dt](auto system){
        system->update(entities, events, dt);
      });
  }

  void configure(EventManager& events){
    _system_order.for_each([&events](auto system){
        system->configure(events);
      });
  }

  void reset(){
    _systems.clear();
    _system_order.reset();
  }

  u64 size(){
    return _systems.size();
  }

  std::string debug_info(){
    std::string str = "Update rates:\n";
    for_each([&str](auto system){
        str += system->get_debug_name();
        str += ":\t";
        str += std::to_string(system->get_framerate());
        str += "\t";
        str += std::to_string(1.0/system->get_timestep_value());
        str += "\n";
      });

    return str;
  }

private:
  std::unordered_map<SystemId, BaseSystem*> _systems;
  memory::CircularList<BaseSystem*> _system_order;
};

}
