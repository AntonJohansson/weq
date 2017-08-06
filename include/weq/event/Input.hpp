#pragma once

#include <weq/system/InputTypes.hpp>

#include <memory>
#include <deque>
#include <map>

// Defined in <weq/system/InputContext.hpp>
class InputContext;

namespace event{

// Event sent out by the input system. Gathers all active inputs,
// sorted into actions, states, and ranges.
struct ActiveInput{
  std::deque<InputAction> actions;
  std::deque<InputState>  states;
  std::map<InputRange, float> ranges;

  // returns true if a given InputAction is active.
  bool has(InputAction action) const {
    return std::find(actions.begin(), actions.end(), action) != actions.end();
  }

  // returns true if a given InputState is active.
  bool has(InputState state) const {
    return std::find(states.begin(), states.end(), state) != states.end();
  }

  // returns true if a given InputRange is active.
  bool has(InputRange range) const {
    return ranges.find(range) != ranges.end();
  }
};

// TODO requried?
struct ChangeInputContext{
  std::shared_ptr<InputContext> context;
};

} // namespace event
