#pragma once

#include <weq/ecs/Fwd.hpp>
#include <weq/ecs/System.hpp>
#include <weq/ecs/Receiver.hpp>
//#include <entityx/entityx.h>
//#include <memory>

//namespace ex = entityx;

namespace weq{class Window;}
// Defined in <weq/event/Input.hpp>
namespace weq::event{struct ActiveInput;}
// Defined in <weq/event/Window.hpp>
namespace weq::event{struct ActiveWindow;}

namespace weq::system{

class Renderer : public System<Renderer>, public Receiver{
public:
  Renderer();
  ~Renderer();

  void configure(EventManager& events) override;

  void update(EntityManager& entities,
              EventManager& events,
              f32 dt) override;

  void receive(event::ActiveInput& event);
  void receive(event::ActiveWindow& event);
private:
  void configure_scene_fbo();
  std::shared_ptr<Window> _window;
};

}
