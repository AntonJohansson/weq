#pragma once

#include <entityx/entityx.h>
#include <memory>

namespace ex = entityx;

class Window;

namespace weq::system{

class Renderer : public ex::System<Renderer>{
public:
  Renderer();
  ~Renderer();

  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

private:
  std::unique_ptr<Window> _window;
};

}
