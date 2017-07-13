#include <weq/system/InputAbstractionLayer.hpp>

#include <glfw/glfw3.h>

void InputAbstractionLayer::register_key(int key, int action, int mods){
  if(_context.is_action(key) && action == GLFW_PRESS){
    _active_input.actions.push_front(_context.action(key));
  }else if(_context.is_state(key) && action != GLFW_RELEASE){
    _active_input.states.push_front(_context.state(key));
  }
}

void InputAbstractionLayer::register_mouse(double x,
                                           double y,
                                           unsigned int x_range,
                                           unsigned int y_range){
  double normalized_x = x/x_range;
  double normalized_y = y/y_range;
}

void InputAbstractionLayer::receive(const event::RegisterInput& event){
}

void InputAbstractionLayer::receive(const event::ChangeInputContext& event){
  _context = event.context;
}
