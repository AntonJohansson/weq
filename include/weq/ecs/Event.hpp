#pragma once

#include <weq/utility/IntegerTypes.hpp>

namespace weq{

class BaseEvent{
public:
  virtual ~BaseEvent(){}
protected:
  static u64 _family_counter;
};

// TEMP -> move to cpp
u64 BaseEvent::_family_counter = 0;

template<typename T>
class Event : public BaseEvent{
public:
  static u64 family(){
    static u64 family = _family_counter++;
    return family;
  }
};

}
