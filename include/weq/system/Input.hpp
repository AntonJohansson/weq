#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

class GLFWwindow;

namespace weq::system{

class Input : public ex::System<Input>{
public:
  Input(){}
  ~Input(){}

  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;
private:
  GLFWwindow* _window_context;
};

}
