#pragma once

#include <weq/state/State.hpp>

#include <unordered_map>

class StateManager{
public:
  StateManager(){}
  ~StateManager(){}

  void add(State state){(void)state;}
  State* active_state(){return _active_state;}
private:
  State* _active_state;
  std::unordered_map<std::string, State> _states;
};
