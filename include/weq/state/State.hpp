#pragma once

#include <weq/system/InputContext.hpp>

#include <memory>
#include <string>

// TODO only supports a single input context

//class InputContext; TODO forward declare?

class State{
public:
  State(const std::string& name, InputContext context)
    : _name(name),
      _input_context(context)
  {}

  const std::string& name(){return _name;}
  InputContext& context(){return _input_context;}

private:
  std::string _name;
  InputContext _input_context;
};
