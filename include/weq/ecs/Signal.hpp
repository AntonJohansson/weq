#pragma once

#include <weq/utility/IntegerTypes.hpp>
#include <weq/memory/PoolAllocator.hpp>
#include <functional>

#include <iostream>

namespace weq{

template<typename>
class Signal;

template<typename R, typename... Args>
class Signal<R(Args...)>{
public:
  using Id = u64;
  using Func = std::function<R(Args...)>;

  struct Node{
    Node(Func f, Node* n = nullptr, Node* p = nullptr)
      : function(f),
        next(n){}

    Func function;
    Node* next;
    Node* prev;
  };

  Signal(u64 size)
    : _head(nullptr),
      _node_pool(size){}

  Node* connect(Func f){
    Node* n = _node_pool.alloc(f);
    //Node* n = new Node(f);
    static int size = 0;
    size++;

    if(_head != nullptr){
      //if(_node_pool.size() == 2){
      if(size == 2){
        n->next = _head;
        n->prev = _head;
        _head->next = n;
        _head->prev = n;
      }else{
        n->next = _head;
        n->prev = _head->prev;

        if(_head->prev){
          // tail -> n
          _head->prev->next = n;
        }

        _head->prev = n;
      }
    }else{
      _head = n;
      _head->next = _head;
      _head->prev = _head;
    }

    // Not sure if this is the best way to handle it :/
    // probably not :)
    return n;
  }

  void disconnect(Node* n){
    n->prev->next = n->next;
    n->next->prev = n->prev;

    if(n == _head){
      _head = (n->next == _head) ? nullptr : n->next;
    }

    _node_pool.dealloc(n);
  }

  void emit(Args&&... args){
    Node* current = _head;

    while(current){
      current->function(std::forward<Args>(args)...);

      current = current->next;
      if(current == _head)break;
    }
  }

private:
  Node* _head;

  memory::PoolAllocator<Node> _node_pool;
};

}
