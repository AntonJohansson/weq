#pragma once

#include <entityx/entityx.h>
#include <glm/glm.hpp> //TODO temp
#include <vector>
#include <memory>

namespace ex = entityx;

class Mesh;

namespace event{
struct DebugDraw;
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

  void receive(const event::DebugDraw& event);
private:
  std::vector<event::DebugDraw> _buffered_events;
  std::vector<ex::Entity> _persistent_entities;
  std::vector<ex::Entity> _frame_entities;
  std::shared_ptr<gl::ShaderProgram> _shader;
};

}
