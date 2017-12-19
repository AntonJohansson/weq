#include <weq/system/Camera.hpp>
#include <weq/component/Camera.hpp>
#include <weq/component/Transform.hpp>
#include <weq/event/Window.hpp>
#include <weq/event/Input.hpp>
#include <weq/event/DebugDraw.hpp>
#include <spdlog/spdlog.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
//#include <glm/gtx/quaternion.hpp>

/* TODO (bug)
   something weird happens when the camera is looking along the "up" vector,
   that results in a black image (empty view-matrix)
*/

// TODO inverted x-movement

namespace{
  glm::vec3 orthogonal(glm::vec3 v){
    static glm::vec3 x_axis = {1,0,0};
    static glm::vec3 y_axis = {0,1,0};
    static glm::vec3 z_axis = {0,0,1};

    float x = glm::abs(v.x);
    float y = glm::abs(v.y);
    float z = glm::abs(v.z);

    glm::vec3 other = (x < y) ? (x < z ? x_axis : z_axis) : (y < z ? y_axis : z_axis);
    return glm::cross(v, other);
  }

  glm::quat rotate_between(glm::vec3 start, glm::vec3 end){
    float cos_theta = glm::dot(start, end);
    float k = glm::sqrt(glm::length2(start)*glm::length2(end));

    if(cos_theta / k == -1){
      return glm::quat(0, glm::normalize(orthogonal(start)));
    }

    return glm::normalize(glm::quat(cos_theta + k, glm::cross(start, end)));
  }
}

namespace weq::system{

Camera::Camera(){
}

Camera::~Camera(){
}

void Camera::configure(ex::EventManager& events){
  spdlog::get("console")->info("camera");
  //events.subscribe<event::WindowUpdate>(*this);
  events.subscribe<event::ActiveWindow>(*this);
  events.subscribe<event::ActiveInput>(*this);
}

void Camera::update(ex::EntityManager& entities,
                    ex::EventManager& events,
                    ex::TimeDelta dt){

  // TODO don't use glm lookat -> manual rot with quaternions.

  auto update_perspective = [dt, this, &events](ex::Entity e,
                                               component::Camera& c,
                                               component::Transform& t){
    // Check aspect ratio
    if(c.inherit_aspect){
      c.aspect_ratio = _aspect_ratio;
      c.update_projection = true;
    }

    // Update look direction for both camera modes.
    if(c.look_mode == LookMode::TARGET){
      //update_target(c, t);
      //c.view = glm::lookAt(t._translate, c.target, c.up);
      look_at(c, t, events);
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
  // TODO This positioning is not being done with quaternions.
  // get the vector from the target to the camera and rotate that one.
  // Utilize "transform" more.

  // Vad händer när target - translate = - up?

  static float r = 10.0f, theta = 45.0f, phi = 45.0f;
  r     += _movement_amount.z;
  // phi increases counter clockwise according to ISO standard.
  // +x -> rotate left.
  phi   -= _delta_cursor.x;
  // theta increases clockwise around the +x axis, +y -> rotate down.
  theta -= _delta_cursor.y;

  // clamp minimum sphere radius
  if(r < 0.01f) r = 0.01f;

  // Update translate vector
  t._translate = camera.target;
  t._translate.x += r*glm::sin(theta)*glm::cos(phi);
  t._translate.y += r*glm::sin(theta)*glm::sin(phi);
  t._translate.z += r*glm::cos(theta);

  //t._translate.x += 0.001f;
  //t._translate.z = 10.0f;

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

void Camera::look_at(component::Camera& camera, component::Transform& t, ex::EventManager& events){
  // This works like update_target... CHAOS I know :(
  glm::vec3 vec = t._translate - camera.target;

  float phi   = -_delta_cursor.x;
  float theta = -_delta_cursor.y;
  static float tot_phi = 45.0f;
  static float tot_theta = 45.0f;
  static float radius = 10.0f;

  tot_phi += phi;
  tot_theta += theta;

  auto right = glm::normalize(glm::cross(glm::normalize(vec), {0,0,1}));
  auto up = glm::normalize(glm::cross(right, vec));

  // Problem då vec || {0,0,1} fixa genom att byta {0,0,1} till nåt annat ortagonalt.
  glm::vec3 v = {0,0,1};
  events.emit(event::DebugDraw(event::DrawType::VECTOR, event::DebugMode::FRAME, right, {0,0,0}, {1, 1, 0, 1}));
  events.emit(event::DebugDraw(event::DrawType::VECTOR, event::DebugMode::FRAME, vec, {0,0,0}, {1, 1, 1, 1}));
  if(glm::length(right) == 0){
    right = glm::normalize(glm::cross(vec, {1,0,0}));
    up = glm::normalize(glm::cross(right, vec));
  }

  // Yaw
  auto rot_1 = glm::rotate(phi, up);
  // Pitch
  auto rot_2 = glm::rotate(theta, right);

  auto rot = rot_2 * rot_1;

  auto new_vec = glm::vec3(rot * glm::vec4(vec, 0));
  t._translate = new_vec + camera.target;

  auto new_right = glm::normalize(glm::cross(new_vec, {0,0,1}));
  auto new_up = glm::normalize(glm::cross(new_right, new_vec));
  camera.up = new_up;
  camera.view = glm::lookAt(t._translate, camera.target, camera.up);

  // Reset mouse vars. :)
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
