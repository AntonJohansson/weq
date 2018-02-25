#pragma once

#include <weq/ecs/Fwd.hpp>
#include <weq/ecs/System.hpp>
#include <weq/ecs/Receiver.hpp>

//#include <entityx/entityx.h>

//namespace ex = entityx;

// Defined in <weq/event/Window.hpp>
namespace weq::event{
struct ActiveWindow;
}
// Defined in <GLFW/glfw3.h>
struct GLFWwindow;

namespace weq::system{

class Input : public System<Input>, public Receiver{
public:
  Input(){}
  ~Input(){}

  void configure(EventManager& events) override;

  void update(EntityManager& entities,
              EventManager& events,
              f32 dt) override;

  void receive(const event::ActiveWindow& event);
private:
  GLFWwindow* _window_context;
};

}
