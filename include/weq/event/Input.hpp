#pragma once

#include <weq/system/InputTypes.hpp>

#include <memory>
#include <deque>
#include <map>

// Defined in <weq/system/InputContext.hpp>
namespace weq::system{
class InputContext;
}

namespace weq::event{

// Event sent out by the input system. Gathers all active inputs,
// sorted into actions, states, and ranges.
struct ActiveInput{
  std::deque<system::InputAction>     actions;
  std::deque<system::InputState>      states;
  std::map<system::InputRange, float> ranges;

  // returns true if a given InputAction is active.
  bool has(system::InputAction action) const {
    return std::find(actions.begin(), actions.end(), action) != actions.end();
  }

  // returns true if a given InputState is active.
  bool has(system::InputState state) const {
    return std::find(states.begin(), states.end(), state) != states.end();
  }

  // returns true if a given InputRange is active.
  bool has(system::InputRange range) const {
    return ranges.find(range) != ranges.end();
  }

  // Consumes (removes) the specified InputAction, if it exists.
  void consume(system::InputAction action){
    auto it = std::find(actions.begin(), actions.end(), action);
    if(it != actions.end()){
      actions.erase(it);
    }
  }

  // Consumes (removes) the specified InputState, if it exists.
  void consume(system::InputState state){
    auto it = std::find(states.begin(), states.end(), state);
    if(it != states.end()){
      states.erase(it);
    }
  }

  // Consumes (removes) the specified InputRange, if it exists.
  void consume(system::InputRange range){
    auto it = ranges.find(range);
    if(it != ranges.end()){
      ranges.erase(it);
    }
  }
};

// TODO requried?
struct ChangeInputContext{
  std::shared_ptr<system::InputContext> context;
};

} // namespace weq::event
