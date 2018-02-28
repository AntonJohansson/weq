#pragma once


#include <weq/ecs/Fwd.hpp>
#include <weq/ecs/System.hpp>
#include <weq/ecs/Receiver.hpp>
//#include <entityx/entityx.h>
#include <glm/glm.hpp> //TODO temp
#include <vector>
#include <memory>

//namespace ex = entityx;

namespace weq{
class Mesh;
}

namespace weq::event{
struct DebugDrawBase;
struct DebugVector;
struct DebugRay;
struct DebugCircle;
class Mesh;
}

namespace weq::gl{
class ShaderProgram;
}

namespace weq::system{

// Requires renderer (right?)
// Should this class construct drawable objects from stuff and add it
// to the drawing queue (top?)
class DebugDraw : public System<DebugDraw>,
                  public Receiver{
public:
  DebugDraw();
  ~DebugDraw();

  void configure(EventManager& events) override;
  void update(EntityManager& entities,
              EventManager& events,
              f32 dt) override;

  void receive(event::DebugVector& event);
  void receive(event::DebugRay& event);
  void receive(event::DebugCircle& event);
private:
  std::vector<std::pair<event::DebugDrawBase, std::shared_ptr<Mesh>>> _buffered_events;
  std::vector<std::pair<EntityId, float>> _timed_entities;
  std::vector<EntityId> _persistent_entities;
  std::shared_ptr<gl::ShaderProgram> _shader;
};

}
