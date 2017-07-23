#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;
namespace event{struct WindowUpdate;}

namespace weq::system{

class Camera : public ex::System<Camera>,
               public ex::Receiver<Camera>{
public:
  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void receive(const event::WindowUpdate& event);
private:
};

}
