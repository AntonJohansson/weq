#include <weq/system/InputAbstractionLayer.hpp>
#include <weq/system/InputRaw.hpp>

#include <glfw/glfw3.h>
#include <iostream>

void InputAbstractionLayer::register_key(int key, int action, int mods){
  if(_context.is_action(key) && action == GLFW_PRESS){
    _active_input.actions.push_front(_context.action(key));
  }else if(_context.is_state(key)){
    if(action == GLFW_PRESS){
      _active_input.states.push_front(_context.state(key));
    }else if(action == GLFW_RELEASE){
      auto it = std::find(_active_input.states.begin(),
                          _active_input.states.end(),
                          _context.state(key));

      if(it != _active_input.states.end()){
        _active_input.states.erase(it);
      }
    }
  }
}

void InputAbstractionLayer::register_mouse(double x,
                                           double y,
                                           unsigned int x_range,
                                           unsigned int y_range){
  if(_context.is_range(raw::Axes::MOUSE_X)){
    static double last_x = 0.0;
    double normalized_x  = -x/x_range; // also flips axis
    double normalized_dx = normalized_x - last_x;
    last_x = normalized_x;

    _active_input.ranges[_context.range(raw::Axes::MOUSE_X)]  = normalized_x;
    _active_input.ranges[InputRange::CURSOR_DX] = normalized_dx;
  }

  if(_context.is_range(raw::Axes::MOUSE_Y)){
    static double last_y = 0.0;
    double normalized_y = -y/y_range; // also flips axis
    double normalized_dy = normalized_y - last_y;
    last_y = normalized_y;

    _active_input.ranges[_context.range(raw::Axes::MOUSE_Y)]  = normalized_y;
    _active_input.ranges[InputRange::CURSOR_DY] = normalized_dy;
  }
}

void InputAbstractionLayer::register_scroll(double xoffset, double yoffset){
  // TODO scroll input is not normalized (undefined range)
  if(_context.is_range(raw::Axes::MOUSE_SCROLL_X)){
    _active_input.ranges[_context.range(raw::Axes::MOUSE_SCROLL_X)] = xoffset;
  }

  if(_context.is_range(raw::Axes::MOUSE_SCROLL_Y)){
    _active_input.ranges[_context.range(raw::Axes::MOUSE_SCROLL_Y)] = yoffset;
  }
}

void InputAbstractionLayer::receive(const event::ChangeInputContext& event){
  _context = event.context;
}
