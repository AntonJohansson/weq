#pragma once

#include <weq/ecs/Event.hpp>
#include <weq/ecs/Signal.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

namespace weq{

// @TODO how should I handle consume?
//enum EventFlag{
//  CONTINUE,
//  CONSUME
//};

using EventSignal = Signal<void(const void*)>;
using EventSignalPtr = std::shared_ptr<EventSignal>;
using EventSignalWeakPtr = std::weak_ptr<EventSignal>;

class EventManager;
class Receiver{
public:
  virtual ~Receiver(){
    for(auto pair : _connections){
      auto& ptr = pair.second.first;
      if(!ptr.expired()){
        ptr.lock()->disconnect(pair.second.second);
      }
    }
  }

private:
  friend class EventManager;
  std::unordered_map<u64, std::pair<EventSignalWeakPtr, EventSignal::Node*>> _connections;
};



class EventManager{
public:

  template<typename E, typename Receiver, u8 prio = 255>
  void subscribe(Receiver& receiver){
    // Get function pointer for reciever of this event type
    void (Receiver::*receive)(const E&) = &Receiver::receive;
    // Get event id for this particular event-type
    u64 event_id = Event<E>::family();
    // Retrieve signal for this event-type
    auto sig = event_signal(event_id);

    // Connect function ptr to signal
    using std::placeholders::_1;
    auto wrapper = EventCallbackWrapper<E>(std::bind(receive, &receiver, _1));
    auto connection = sig->connect(wrapper, prio);


    // Connect Event to receiver (needed for unsubbing etc.)
    receiver._connections[event_id] = std::make_pair(EventSignalWeakPtr(sig), connection);
  }

  template<typename E, typename Receiver>
  void unsubscribe(Receiver& receiver){
    u64 event_id = Event<E>::family();

    // Assumes receiver is a parent of Receiver
    auto it = receiver._connections.find(event_id);
    if(it != receiver._connections.end()){
      auto& ptr = it->second.first;
      if(!ptr.expired()){
        ptr.lock()->disconnect(it->second.second);
      }
      receiver._connections.erase(event_id);
    }else{
      // receiver not subscribed to event
    }
  }

  template<typename E, typename... Args>
  void emit(Args&&... args){
    // Create the event type
    E event = E(std::forward<Args>(args)...);
    // Get event id
    u64 event_id = Event<E>::family();
    // Get signal
    auto sig = event_signal(event_id);
    // Emit
    sig->emit(&event);
  }
private:
  EventSignalPtr event_signal(u64 event_id){
    auto it = _signal_map.find(event_id);

    if(it != _signal_map.end()){
      return it->second;
    }else{
      _signal_map[event_id] = std::make_shared<EventSignal>();
      return _signal_map[event_id];
    }
  }

  // Convert void(void*) to void(const E&)
  template <typename E>
  struct EventCallbackWrapper {
    explicit EventCallbackWrapper(std::function<void(const E &)> callback) : callback(callback) {}
    void operator()(const void *event) { callback(*(static_cast<const E*>(event))); }
    std::function<void(const E &)> callback;
  };


  std::unordered_map<u64, EventSignalPtr> _signal_map;
};

}
