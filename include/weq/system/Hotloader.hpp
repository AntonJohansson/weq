#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

// Forward declarations
namespace weq::event{
struct Track;
}

namespace weq::system{

class Hotloader : public ex::System<Hotloader>, public ex::Receiver<Hotloader>{
public:
  Hotloader();
  ~Hotloader();

  void configure(ex::EventManager& events) override;

  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void receive(const event::Track& event);
private:
};

} // namespace weq::system
