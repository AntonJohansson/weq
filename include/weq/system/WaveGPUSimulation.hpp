#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

// Defined in <weq/event/Input.hpp>
namespace weq::event{struct ActiveInput;}
// Defined in <weq/event/UI.hpp>
namespace weq::event{struct UI;}

namespace weq::system{

class WaveGPUSimulation : public ex::System<WaveGPUSimulation>,
                          public ex::Receiver<WaveGPUSimulation>{
public:
  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void add_ui(ex::EntityManager& entities, ex::EventManager& events);

  void receive(const event::ActiveInput& event);
private:
  bool _ui_created = false;
  ex::Entity _ui;
};

}
