#pragma once

#include <entityx/entityx.h>
#include <memory>

namespace ex = entityx;

class Window;
// Defined in <weq/event/Input.hpp>
namespace event{struct ActiveInput;}

namespace weq::system{

class UserInterface : public ex::System<UserInterface>, public ex::Receiver<UserInterface>{
public:
  UserInterface();
  ~UserInterface();

  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void receive(const event::ActiveInput& event);
};

}
