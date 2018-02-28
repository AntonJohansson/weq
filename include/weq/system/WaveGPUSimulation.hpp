#pragma once

#include <weq/ecs/Fwd.hpp>
#include <weq/ecs/System.hpp>
#include <weq/ecs/Receiver.hpp>
//#include <entityx/entityx.h>

//namespace ex = entityx;

// Defined in <weq/event/Input.hpp>
namespace weq::event{struct ActiveInput;}
// Defined in <weq/event/UI.hpp>
namespace weq::event{struct UI;}

namespace weq::system{

class WaveGPUSimulation : public System<WaveGPUSimulation>,
                          public Receiver{
public:
  void configure(EventManager& events) override;

  void update(EntityManager& entities,
              EventManager& events,
              f32 dt) override;

  void add_ui(EntityManager& entities, EventManager& events);

  void receive(event::ActiveInput& event);
private:
  bool _ui_created = false;
  EntityId _ui;
};

}
