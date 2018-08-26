#pragma once

#include <weq/utility/NumberTypes.hpp>
#include <weq/memory/CircularList.hpp>
//#include <functional>

namespace weq{

template<typename>
class Signal;

template<typename R, typename... Args>
class Signal<R(Args...)>{
public:
  using Id   = u64;
  using Func = std::function<R(Args...)>;
  using Node = typename memory::CircularList<Func>::Node;

  Node* connect(Func f, u8 prio = -1 /* max prio */){
    Node* n = _signal_list.add(f, prio);
    return n;
  }

  void disconnect(Node* n){
    _signal_list.remove(n);
  }

  void emit(Args&&... args){
    //// @TODO better impl., currently reimplementing code from
    //// CirularList and making assumptions about list structure
    //auto* head = _signal_list.head();
    //auto* current = head;

    //while(current){
    //  R return_val = current->data(std::forward<Args>(args)...);

    //  // Check return val, if func returns false we stop event emission
    //  // => events can be consumed
    //  // @TODO better impl.?
    //  if(_check_return_func && !_check_return_func(return_val)){
    //    break;
    //  }

    //  current = current->next;
    //  if(current == head)break;
    //}

    _signal_list.for_each([&](auto& func){
        func(std::forward<Args>(args)...);
      });
  }

  // @TODO correct method of impl.?
  //void set_check_return_function(std::function<bool(R)> f){
  //  _check_return_func = f;
  //}

  void reset(){
    _signal_list.reset();
  }

  void debug_print(){
    _signal_list.debug_print();
  }

private:
  //std::function<bool(R)>     _check_return_func;
  memory::CircularList<Func> _signal_list;
};

}
