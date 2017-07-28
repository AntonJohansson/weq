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

Camera::Camera(){
}

Camera::~Camera(){
}

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
      c.view = glm::lookAt(c.position, c.target, c.up);
    }else if(c.look_mode == LookMode::DIRECTION){
      update_direction(c, t);
      c.view = glm::lookAt(t._translate, t._translate + t._direction, c.up);
    }

    //TODO Move this? model needs to be built before render anc camera pass
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

  t.rotate({glm::radians(_delta_cursor.y), glm::radians(_delta_cursor.x), 0});

  //t.transform = glm::rotate(t.transform, glm::radians(10.0f*_dx), local_up);
  //t.transform = glm::rotate(t.transform, glm::radians(10.0f*_dy), right);
  //t.transform = glm::translate(t.transform, _translate);

  t._translate += (right*_movement_amount.x +
                   local_up*_movement_amount.y +
                   t._direction*_movement_amount.z);
  _delta_cursor = {0,0};
  _movement_amount = {0,0,0};
}

void Camera::receive(const event::WindowUpdate& event){
  _aspect_ratio = event.aspect_ratio;
}

void Camera::receive(const event::ActiveInput& event){
  // Only update mouse camera direction when mouse is down
  if(event.has(InputState::CURSOR_DOWN)){
    // Update mouse delta position
    if(event.has(InputRange::CURSOR_DX) && event.has(InputRange::CURSOR_DY)){
      _delta_cursor.x = 10.0f*event.ranges.at(InputRange::CURSOR_DX);
      _delta_cursor.y = 10.0f*event.ranges.at(InputRange::CURSOR_DY);
    }
  }

  // Update camera distance with scroll
  if(event.has(InputRange::CURSOR_SCROLL_X) && event.has(InputRange::CURSOR_SCROLL_Y)){
    double x = event.ranges.at(InputRange::CURSOR_SCROLL_X);
    double y = event.ranges.at(InputRange::CURSOR_SCROLL_Y);
    _movement_amount.z = y;
  }

  // Update camera movement vector
  float speed = 0.05f;
  if(event.has(InputState::MOVE_LEFT)){
    _movement_amount.x = -speed;
  }if(event.has(InputState::MOVE_RIGHT)){
    _movement_amount.x = +speed;
  }if(event.has(InputState::MOVE_FORWARD)){
    _movement_amount.z = +speed;
  }if(event.has(InputState::MOVE_BACK)){
    _movement_amount.z = -speed;
  }if(event.has(InputState::MOVE_UP)){
    _movement_amount.y = +speed;
  }if(event.has(InputState::MOVE_DOWN)){
    _movement_amount.y = -speed;
  }
}

}
