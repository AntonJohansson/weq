#pragma once

#include <entityx/entityx.h>
#include <memory>

namespace ex = entityx;

class Window;
namespace event{struct ActiveInput;}

namespace weq::system{

class Renderer : public ex::System<Renderer>, public ex::Receiver<Renderer>{
public:
  Renderer();
  ~Renderer();

  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void receive(const event::ActiveInput& event);

private:
  std::unique_ptr<Window> _window;
};

}
