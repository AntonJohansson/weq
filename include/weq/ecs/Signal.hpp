#pragma once

#include <weq/utility/NumberTypes.hpp>
#include <weq/memory/CircularList.hpp>
#include <functional>

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
    _signal_list.for_each([&](auto* node){
        node->data(std::forward<Args>(args)...);
      });
  }

  void reset(){
    _signal_list.reset();
  }

  void debug_print(){
    _signal_list.debug_print();
  }

private:
  memory::CircularList<Func> _signal_list;
};

}
