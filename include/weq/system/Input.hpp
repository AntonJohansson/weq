#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

// Defined in <weq/event/Window.hpp>
namespace event{struct ActiveWindow;}
// Defined in <GLFW/glfw3.h>
struct GLFWwindow;

namespace weq::system{

class Input : public ex::System<Input>, public ex::Receiver<Input>{
public:
  Input(){}
  ~Input(){}

  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void receive(const event::ActiveWindow& event);
private:
  GLFWwindow* _window_context;
};

}
