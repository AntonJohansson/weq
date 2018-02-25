#pragma once

#include <weq/ecs/Fwd.hpp>
#include <weq/ecs/System.hpp>
#include <weq/ecs/Receiver.hpp>
//#include <entityx/entityx.h>

//namespace ex = entityx;

// Forward declarations
namespace weq::event{
struct Track;
}

namespace weq::system{

class Hotloader : public System<Hotloader>, public Receiver{
public:
  Hotloader();
  ~Hotloader();

  void configure(EventManager& events) override;

  void update(EntityManager& entities,
              EventManager& events,
              f32 dt) override;

  void receive(const event::Track& event);
private:
};

} // namespace weq::system
