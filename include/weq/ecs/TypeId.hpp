#pragma once

#include <weq/utility/NumberTypes.hpp>

namespace weq{

template<typename Base>
class BaseTypeId{
public:
  virtual ~BaseTypeId(){}
protected:
  static u64 _type_counter;
};

template<typename Base, typename T>
class TypeId : public BaseTypeId<Base>{
public:
  static u64 id(){
    static u64 id = _type_counter++;
    return id;
  }
};

}
