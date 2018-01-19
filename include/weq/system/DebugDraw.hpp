#pragma once


#include <entityx/entityx.h>
#include <glm/glm.hpp> //TODO temp
#include <vector>
#include <memory>

namespace ex = entityx;

class Mesh;

namespace event{
struct DebugDrawBase;
struct DebugVector;
struct DebugRay;
struct DebugCircle;
class Mesh;
}

namespace gl{
class ShaderProgram;
}

namespace weq::system{

// Requires renderer (right?)
// Should this class construct drawable objects from stuff and add it
// to the drawing queue (top?)
class DebugDraw : public ex::System<DebugDraw>,
                  public ex::Receiver<DebugDraw>{
public:
  DebugDraw();
  ~DebugDraw();

  void configure(ex::EventManager& events) override;
  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void receive(const event::DebugVector& event);
  void receive(const event::DebugRay& event);
  void receive(const event::DebugCircle& event);
private:
  std::vector<std::pair<event::DebugDrawBase, std::shared_ptr<Mesh>>> _buffered_events;
  std::vector<std::pair<ex::Entity, float>> _timed_entities;
  std::vector<ex::Entity> _persistent_entities;
  std::shared_ptr<gl::ShaderProgram> _shader;
};

}
