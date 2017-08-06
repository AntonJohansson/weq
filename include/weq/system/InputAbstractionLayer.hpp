#pragma once

#include <entityx/entityx.h>

#include <memory>

namespace ex = entityx;

// TODO this doesnt need to be a class. Totally useless.
// TODO move event handling to input system and forward.

// Defined in <weq/event/Input.hpp>
namespace event{
struct ActiveInput;
struct ChangeInputContext;
}

// Defined in <weq/system/InputContext.hpp>
class InputContext;

class InputAbstractionLayer : public ex::Receiver<InputAbstractionLayer>{
public:
  InputAbstractionLayer(std::shared_ptr<InputContext> context);

  void register_key(int key, int action, int mods);
  void register_mouse(double x, double y, unsigned int x_range, unsigned int y_range);
  void register_scroll(double xoffset, double yoffset);

  void receive(const event::ChangeInputContext& event);

  const event::ActiveInput& get_active();
  void clear();
};
