#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

// Defined in <weq/event/Input.hpp>
namespace event{struct ActiveInput;}
// Defined in <weq/event/UI.hpp>
namespace event{struct UI;}

namespace weq::system{

class WaveGPUSimulation : public ex::System<WaveGPUSimulation>,
                          public ex::Receiver<WaveGPUSimulation>{
public:
  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void add_ui();

  void receive(const event::ActiveInput& event);
private:
};

}
