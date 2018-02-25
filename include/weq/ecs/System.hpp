#pragma once

#include <weq/utility/NumberTypes.hpp>

namespace weq{

class SystemManager;
class EntityManager;
class EventManager;
struct Managers{
  SystemManager* system;
  EntityManager* entity;
  EventManager*  event;
};

class BaseSystem{
public:
  virtual ~BaseSystem(){}

  virtual void configure(Managers&){}
  virtual void update(Managers&, f32) = 0;

  static u64 _type_counter;
};

// @TODO move to cpp
u64 BaseSystem::_type_counter = 0;

template<typename S>
class System : public BaseSystem{
public:
  virtual ~System(){}
private:
  friend class SystemManager;

  static u64 id(){
    static u64 id = _type_counter++;
    return id;
  }
};

}
