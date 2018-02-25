#pragma once

#include <weq/utility/NumberTypes.hpp>

namespace weq{

class BaseEvent{
public:
  virtual ~BaseEvent(){}
protected:
  static u64 _type_counter;
};

template<typename T>
class Event : public BaseEvent{
public:
  static u64 id(){
    static u64 id = _type_counter++;
    return id;
  }
};

}
