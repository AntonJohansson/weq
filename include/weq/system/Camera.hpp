#pragma once

#include <entityx/entityx.h>
#include <glm/glm.hpp> //TODO temp

namespace ex = entityx;

namespace component{
// Defined in <weq/component/Camera.hpp>
struct Camera;
// Defined in <weq/component/Camera.hpp>
struct Transform;
}

namespace event{
// Defined in <weq/event/Window.hpp>
struct ActiveWindow;
// Defined in <weq/event/Input.hpp>
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

  void update_target(component::Camera& camera, component::Transform& t);
  void update_direction(component::Camera& camera, component::Transform& t);
  void look_at(component::Camera& camera, component::Transform& t);

  void receive(const event::ActiveWindow& event);
  void receive(const event::ActiveInput& event);

private:
  // TODO: Not sure about these ones.
  glm::vec2 _delta_cursor;
  glm::vec3 _movement_amount;
  float _aspect_ratio;
  glm::vec3 _translate = {0,0,0};
};

}
