#pragma once

#include <entityx/entityx.h>
#include <memory>

namespace ex = entityx;

class Window;
// Defined in <weq/event/Input.hpp>
namespace event{struct ActiveInput;}
// Defined in <weq/event/Window.hpp>
namespace event{struct ActiveWindow;}

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
  void receive(const event::ActiveWindow& event);
private:
  void configure_scene_fbo();
  std::shared_ptr<Window> _window;
};

}
