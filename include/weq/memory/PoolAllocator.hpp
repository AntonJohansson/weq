#pragma once

#include <weq/utility/IntegerTypes.hpp>
#include <utility>
#include <queue>
#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <cassert>

namespace weq::memory{

template<typename T>
class PoolAllocator{
public:
  explicit PoolAllocator(u64 size)
    : _size(0),
      _max_size(size){
    _memory_block = std::malloc(_max_size * sizeof(T));

    setup_free_list();
  }

  ~PoolAllocator(){
    std::free(_memory_block);
  }

  template<typename... Args>
  T* alloc(Args&&... args){
    T* ptr = _free_list.front();
    _free_list.pop();

    //*ptr = T(std::forward<Args>(args)...);
    ptr = new (ptr) T(std::forward<Args>(args)...);

    _size++;

    return ptr;
  }

  void dealloc(T* t){
    _free_list.push(t);
    _size--;
  }

  u64 size(){return _size;}
  u64 max_size(){return _max_size;}
  u64 free_size(){return _free_list.size();}

private:
  void setup_free_list(){
    T* ptr = static_cast<T*>(_memory_block);

    for(u64 i = 0; i < _max_size; i++){
      _free_list.push(ptr);
      ptr++;
    }
  }

  u64 _size;
  u64 _max_size;
  std::queue<T*> _free_list;

  void* _memory_block;
};

}
