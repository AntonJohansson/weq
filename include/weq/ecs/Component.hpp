#pragma once

#include <weq/utility/NumberTypes.hpp>

namespace weq{

class BaseComponentTypeId{
public:
  virtual ~BaseComponentTypeId(){}
protected:
  static u64 _type_counter;
};

template<typename T>
class ComponentTypeId : public BaseComponentTypeId{
public:
  static u64 id(){
    static u64 id = _type_counter++;
    return id;
  }
};


}
