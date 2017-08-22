#include <weq/system/Camera.hpp>
#include <weq/component/Camera.hpp>
#include <weq/component/Transform.hpp>
#include <weq/event/Window.hpp>
#include <weq/event/Input.hpp>

#include <spdlog/spdlog.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

/* TODO (bug)
   something weird happens when the camera is looking along the "up" vector,
   that results in a black image (empty view-matrix)
*/

// TODO inverted x-movement

namespace weq::system{

Camera::Camera(){
}

Camera::~Camera(){
}

void Camera::configure(ex::EventManager& events){
  //events.subscribe<event::WindowUpdate>(*this);
  events.subscribe<event::ActiveWindow>(*this);
  events.subscribe<event::ActiveInput>(*this);
}

void Camera::update(ex::EntityManager& entities,
                    ex::EventManager& events,
                    ex::TimeDelta dt){

  auto update_perspective = [dt, this](ex::Entity e,
                                         component::Camera& c,
                                         component::Transform& t){
    // Check aspect ratio
    if(c.inherit_aspect){
      c.aspect_ratio = _aspect_ratio;
      c.update_projection = true;
    }

    // Update look direction for both camera modes.
    if(c.look_mode == LookMode::TARGET){
      update_target(c, t);
      c.view = glm::lookAt(t._translate, c.target, c.up);
    }else if(c.look_mode == LookMode::DIRECTION){
      update_direction(c, t);
      c.view = glm::lookAt(t._translate, t._translate + t._direction, c.up);
    }

    //TODO Move this? model needs to be built before render anc camera pass
    //c.normal_matrix = glm::transpose(glm::inverse(c.view*t.transform));

    if(c.update_projection){
      c.projection = glm::perspective(glm::radians(c.fov), c.aspect_ratio, c._near, c._far);
      c.update_projection = false;
    }
  };

  entities.each<component::Camera, component::Transform>(update_perspective);
}

void Camera::update_target(component::Camera& camera, component::Transform& t){
  static float r = 10.0f, theta = 45.0f, phi = 45.0f;
  r     += _movement_amount.z;
  // phi increases counter clockwise according to ISO standard.
  // +x -> rotate left.
  phi   -= _delta_cursor.x;
  // theta increases clockwise around the +x axis, +y -> rotate down.
  theta -= _delta_cursor.y;

  // clamp minimum sphere radius
  if(r < 0.01f) r = 0.01f;

  t._translate = camera.target;
  t._translate.x += r*glm::sin(theta)*glm::cos(phi);
  t._translate.y += r*glm::sin(theta)*glm::sin(phi);
  t._translate.z += r*glm::cos(theta);

  _delta_cursor = {0,0};
  _movement_amount = {0,0,0};
}

void Camera::update_direction(component::Camera& camera, component::Transform& t){
  auto right    = glm::cross(t._direction, camera.up);
  auto local_up = glm::cross(right, t._direction);

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

void Camera::receive(const event::ActiveWindow& event){
  _aspect_ratio = event.window->aspect_ratio();
}

void Camera::receive(const event::ActiveInput& event){
  // Only update mouse camera direction when mouse is down
  if(event.has(InputState::CURSOR_DOWN)){
    // Update mouse delta position
    if(event.has(InputRange::CURSOR_DX) && event.has(InputRange::CURSOR_DY)){
      _delta_cursor.x = event.ranges.at(InputRange::CURSOR_DX);
      _delta_cursor.y = event.ranges.at(InputRange::CURSOR_DY);
    }
  }

  // Update camera distance with scroll
  if(event.has(InputRange::CURSOR_SCROLL_X) && event.has(InputRange::CURSOR_SCROLL_Y)){
    //double x = event.ranges.at(InputRange::CURSOR_SCROLL_X);
    _movement_amount.z = -0.5*event.ranges.at(InputRange::CURSOR_SCROLL_Y);
  }

  /* First person camera
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
  */
}

}
