#pragma once

#include <weq/system/InputTypes.hpp>
#include <weq/system/InputContext.hpp>

#include <deque>
#include <map>

namespace event{

struct RegisterInput{
  RegisterInput(InputAction action = InputAction::UNKNOWN,
                     int key = -1,
                     int key_state = -1)
    : action(action),
      key(key),
      key_state(key_state){
  }

  InputAction action;
  int key;
  int key_state;
};

struct ActiveInput{
  std::deque<InputAction> actions;
  std::deque<InputState>  states;
  std::map<InputRange, float> ranges;

  bool has(InputAction action) const {
    return std::find(actions.begin(), actions.end(), action) != actions.end();
  }

  bool has(InputState state) const {
    return std::find(states.begin(), states.end(), state) != states.end();
  }

  bool has(InputRange range) const {
    return ranges.find(range) != ranges.end();
  }
};

struct ChangeInputContext{
  InputContext context;
};

}
