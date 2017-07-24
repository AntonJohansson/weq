#include <weq/system/Camera.hpp>
#include <weq/component/Camera.hpp>
#include <weq/component/Transform.hpp>
#include <weq/event/Window.hpp>
#include <weq/event/RegisterInput.hpp>

#include <spdlog/spdlog.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace weq::system{

void Camera::configure(ex::EventManager& events){
  events.subscribe<event::WindowUpdate>(*this);
  events.subscribe<event::ActiveInput>(*this);
}

void Camera::update(ex::EntityManager& entities,
                    ex::EventManager& events,
                    ex::TimeDelta dt){

  auto update_perspective = [dt, this](ex::Entity e,
                                         component::Camera& c,
                                         component::Transform& t){
    if(c.look_mode == LookMode::TARGET){
      update_target(c);
      c.position = t.transform * glm::vec4(c.position, 1.0f);
      c.direction = t.transform * glm::vec4(c.direction, 0.0f);
      c.view = glm::lookAt(c.position, c.target, c.up);
    }else if(c.look_mode == LookMode::DIRECTION){
      update_direction(c, t);
      c.position = t.transform * glm::vec4(c.position, 1.0f);
      c.direction = t.transform * glm::vec4(c.direction, 0.0f);
      c.view = glm::lookAt(c.position, c.position + c.direction, c.up);
    }

    //c.normal_matrix = glm::transpose(glm::inverse(c.view*t.transform));

    if(c.update_projection){
      c.projection = glm::perspective(glm::radians(c.fov), c.aspect_ratio, c.near, c.far);
      c.update_projection = false;
    }
  };

  entities.each<component::Camera, component::Transform>(update_perspective);
}

void Camera::update_target(component::Camera& camera){
}

void Camera::update_direction(component::Camera& camera, component::Transform& t){
  auto right    = glm::cross(camera.direction, camera.up);
  auto local_up = glm::cross(right, camera.direction);

  t.transform = glm::rotate(t.transform, 10.0f*_dx, local_up);
  t.transform = glm::rotate(t.transform, 10.0f*_dy, right);

  t.transform = glm::translate(t.transform, _translate);
  _translate = {0,0,0};
}

void Camera::receive(const event::WindowUpdate& event){
  _aspect_ratio = event.aspect_ratio;
}

void Camera::receive(const event::ActiveInput& event){
  if(event.has(InputRange::CURSOR_X) && event.has(InputRange::CURSOR_Y)){
    static float last_x = 0.0f;
    static float last_y = 0.0f;

    const float& x = event.ranges.at(InputRange::CURSOR_X);
    const float& y = event.ranges.at(InputRange::CURSOR_Y);

    _dx = x - last_x;
    _dy = y - last_y;

    last_x = x;
    last_y = y;
  }

  float speed = 0.05f;
  if(event.has(InputState::MOVE_LEFT)){
    _translate.x = -speed;
  }if(event.has(InputState::MOVE_RIGHT)){
    _translate.x = +speed;
  }if(event.has(InputState::MOVE_FORWARD)){
    _translate.z = +speed;
  }if(event.has(InputState::MOVE_BACK)){
    _translate.z = -speed;
  }if(event.has(InputState::MOVE_UP)){
    _translate.y = +speed;
  }if(event.has(InputState::MOVE_DOWN)){
    _translate.y = -speed;
  }
}

}
