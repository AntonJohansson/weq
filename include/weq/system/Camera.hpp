#pragma once

#include <entityx/entityx.h>
#include <glm/glm.hpp> //TODO temp

namespace ex = entityx;

namespace component{
struct Camera;
struct Transform;
}

namespace event{
struct WindowUpdate;
struct ActiveInput;
}

namespace weq::system{

class Camera : public ex::System<Camera>,
               public ex::Receiver<Camera>{
public:
  Camera();
  ~Camera();

  void configure(ex::EventManager& events) override;
  void update(ex::EntityManager& entities,
              ex::EventManager& events,
              ex::TimeDelta dt) override;

  void update_target(component::Camera& camera);
  void update_direction(component::Camera& camera, component::Transform& t);

  void receive(const event::WindowUpdate& event);
  void receive(const event::ActiveInput& event);

private:
  glm::vec2 _delta_cursor;
  glm::vec3 _movement_amount;
  float _aspect_ratio;
  glm::vec3 _translate = {0,0,0};
};

}
