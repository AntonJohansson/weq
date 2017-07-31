#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

namespace event{struct ActiveInput;}

namespace weq::system{

class WaveGPUSimulation : public ex::System<WaveGPUSimulation>,
                          public ex::Receiver<WaveGPUSimulation>{
public:
  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;
  void receive(const event::ActiveInput& event);
private:
};

}
