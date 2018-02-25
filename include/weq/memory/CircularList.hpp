#pragma once

#include <weq/memory/PoolAllocator.hpp>
#include <weq/utility/NumberTypes.hpp>
// temp
#include <functional>
#include <iostream>
#include <iomanip>

namespace weq::memory{

template<typename T>
class CircularList{
public:
  struct Node{
    Node(T data, u8 priority)
      : data(data),
        priority(priority),
        next(nullptr),
        prev(nullptr){}

    T  data;
    u8 priority;
    Node* next;
    Node* prev;

    // @TODO temp used for debugging
    u64 id;
  };

  CircularList()
    : _head(nullptr),
      _size(0),
      _node_pool(16, RESIZE){}

  Node* add(T data, u8 priority = -1 /* max prio */){
    Node* n = _node_pool.alloc(data, priority);
    static u64 id_counter = 0;
    n->id = id_counter++;

    // @TODO simplify :)
    if(priority == -1){
      // Don't care about prio
      if(_head != nullptr){
        if(_size == 1){
          // Only head
          n->next = _head;
          n->prev = _head;
          _head->next = n;
          _head->prev = n;
        }else{
          // Head + other node
          n->next = _head;
          n->prev = _head->prev;

          if(_head->prev){
            // tail -> n
            _head->prev->next = n;
          }

          _head->prev = n;
        }
      }else{
        // No node
        _head = n;
        _head->next = _head;
        _head->prev = _head;
      }
    }else{
      // Care about prio
      // insert 1
      // 256          ->(1)| 256
      // 0 | 256      -> 0 |(1)| 256
      // 1 | 1 | 256  -> 1 | 1 |(1)| 256
      // 0            -> 0 |(1)
      if(_head != nullptr){
        Node* insert_before = _head;

        int index = 0;
        while(insert_before->priority <= n->priority){
          if(insert_before->next){
            index++;
            insert_before = insert_before->next;

            if(insert_before == _head)break;
          }else break;
        }

        n->next = insert_before;
        n->prev = insert_before->prev;

        insert_before->prev->next = n;
        insert_before->prev = n;

        if(insert_before == _head && index == 0)_head = n;
      }else{
        // No node
        _head = n;
        _head->next = _head;
        _head->prev = _head;
      }
    }

    _size++;

    // Not sure if this is the best way to handle it :/
    // probably not :)
    return n;
  }

  void remove(Node* n){
    // @TODO n = nullptr while cause crash
    _size--;

    n->prev->next = n->next;
    n->next->prev = n->prev;

    if(n == _head){
      _head = (n->next == _head) ? nullptr : n->next;
    }

    _node_pool.dealloc(n);
  }

  void for_each(std::function<void(Node*)> func){
    auto* current = _head;

    while(current){
      func(current);

      current = current->next;
      if(current == _head)break;
    }
  }

  void reset(){
    _head = nullptr;
    _size = 0;
    _node_pool.reset();
  }

  Node* head(){
    return _head;
  }

  u64 size(){
    return _size;
  }

  void debug_print(){
    Node* current = _head;

    std::cout << std::setw(16) << "ID" << " | "
              << std::setw(16) << "PRIO" << "\n";
    while(current){
      std::cout << std::setw(16) << current->id << " | "
                << std::setw(16) << (int)current->priority << "\n";

      current = current->next;
      if(current == _head)break;
    }
  }

private:
  Node* _head;
  u64   _size;

  PoolAllocator<Node> _node_pool;
};

};
