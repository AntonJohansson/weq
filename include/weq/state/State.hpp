#pragma once

#include <weq/system/InputContext.hpp>

#include <memory>
#include <string>

// TODO only supports a single input context

//class InputContext; TODO forward declare?

namespace weq::state{

class State{
public:
  State(const std::string& name, system::InputContext context)
    : _name(name),
      _input_context(context)
  {}

  const std::string& name(){return _name;}
  system::InputContext& context(){return _input_context;}

private:
  std::string _name;
  system::InputContext _input_context;
};

} // namespace weq::state
