#pragma once

#include <weq/event/RegisterInput.hpp>

#include <entityx/entityx.h>

#include <unordered_map>
#include <deque>

namespace ex = entityx;

class InputAbstractionLayer : public ex::Receiver<InputAbstractionLayer>{
public:
  InputAbstractionLayer(InputContext context)
    : _context(context){
  }

  void register_key(int key, int action, int mods);
  void register_mouse(double x, double y, unsigned int x_range, unsigned int y_range);
  void register_scroll(double xoffset, double yoffset);

  void receive(const event::ChangeInputContext& event);

  const event::ActiveInput& get_active(){return _active_input;}
  void clear(){
    _active_input.actions.clear();
    //_active_input.states.clear();
    _active_input.ranges.clear();
  }
private:
  event::ActiveInput _active_input;
  InputContext _context;
};
