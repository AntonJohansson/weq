#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

namespace event{struct ActiveInput;}

namespace weq::system{

class WaveSimulation : public ex::System<WaveSimulation>,
                       public ex::Receiver<WaveSimulation>{
public:
  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;
  void receive(const event::ActiveInput& event);
private:
};

}
