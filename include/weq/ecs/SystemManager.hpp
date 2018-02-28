#pragma once

#include <weq/ecs/System.hpp>
#include <weq/utility/NumberTypes.hpp>
#include <weq/memory/CircularList.hpp>
#include <unordered_map>
#include <sstream>
#include <iomanip>
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

  template<typename S>
  S* get(){
    // @TODO check if we actually have the system
    BaseSystem* base = _systems[S::id()];
    return static_cast<S*>(base);
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
    std::stringstream stream;
    stream << std::setw(20) << std::left << "Update Rates:"
           << std::setw(8) << std::left << "current"
           << std::setw(8) << std::right << "max\n";
    for_each([&stream](auto system){
        stream << std::setw(4) << " " << std::setw(16) << std::left << system->get_debug_name()
               << std::setw(4) << " " << std::setw(8)  << std::setprecision(5) << system->get_framerate()
               << std::setw(4) << " " << std::setw(8)  << std::setprecision(5) << 1.0/system->get_timestep_value()
               << "\n";
      });

    return stream.str();
  }

private:
  std::unordered_map<SystemId, BaseSystem*> _systems;
  memory::CircularList<BaseSystem*> _system_order;
};

}
