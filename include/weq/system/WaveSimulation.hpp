#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

namespace weq::system{

class WaveSimulation : public ex::System<WaveSimulation>{
public:
  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

private:
};

}
