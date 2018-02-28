#pragma once

#include <weq/ecs/Fwd.hpp>
#include <weq/ecs/System.hpp>
#include <weq/ecs/Receiver.hpp>

#include <memory>
//#include <entityx/entityx.h>

//namespace ex = entityx;

class Window;
// Defined in <weq/event/Input.hpp>
namespace weq::event{struct ActiveInput;}
// Defined in <weq/event/Window.hpp>
namespace weq::event{struct ActiveWindow;}

namespace weq::system{

class UserInterface : public System<UserInterface>, public Receiver{
public:
  UserInterface();
  ~UserInterface();

  void configure(EventManager& events) override;

  void update(EntityManager& entities,
              EventManager& events,
              f32 dt) override;

  void receive(event::ActiveInput& event);
  void receive(event::ActiveWindow& event);
};

}
