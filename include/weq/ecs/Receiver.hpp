#pragma once

#include <weq/ecs/Fwd.hpp>
// @TODO cannot forward declare Node* hmm :(
#include <weq/ecs/Signal.hpp>

//#include <unordered_map>

namespace weq{
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
}
