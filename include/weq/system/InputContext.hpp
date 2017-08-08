#pragma once

#include <weq/system/InputTypes.hpp>
#include <unordered_map>

template<typename T, typename U>
using InputMap = std::unordered_map<T, U>;

class InputContext{
public:

  InputContext(InputMap<int, InputAction> action,
               InputMap<int, InputState> state,
               InputMap<int, InputRange> range)
    : _action_map(action),
      _state_map(state),
      _range_map(range){
  }

  void map(int input, InputAction action){_action_map[input] = action;}
  void map(int input, InputState state){_state_map[input] = state;}
  void map(int input, InputRange range){_range_map[input] = range;}
  void map(InputMap<int, InputAction> action_map){_action_map = action_map;}
  void map(InputMap<int, InputState> state_map){_state_map = state_map;}
  void map(InputMap<int, InputRange> range_map){_range_map = range_map;}

  bool is_action(int key){return _action_map.find(key) != _action_map.end();}
  bool is_state(int key){return _state_map.find(key) != _state_map.end();}
  bool is_range(int key){return _range_map.find(key) != _range_map.end();}

  const InputAction& action(int key){return _action_map[key];}
  const InputState& state(int key){return _state_map[key];}
  const InputRange& range(int key){return _range_map[key];}

private:
  InputMap<int, InputAction> _action_map;
  InputMap<int, InputState> _state_map;
  InputMap<int, InputRange> _range_map;
};
