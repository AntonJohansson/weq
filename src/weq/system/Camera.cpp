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
#include <glm/gtc/constants.hpp>
#include <glm/ext.hpp>

#include <algorithm>
#include <cmath>

#ifdef _WIN32
#undef near
#undef far
#endif

//#include <glm/gtx/quaternion.hpp>

/* TODO (bug)
   something weird happens when the camera is looking along the "up" vector,
   that results in a black image (empty view-matrix)
*/

// TODO inverted x-movement

namespace{
  void draw_mat(glm::mat4 m){
    spdlog::get("console")->info(
      "\n{}\t{}\t{}\t{}\n{}\t{}\t{}\t{}\n{}\t{}\t{}\t{}\n{}\t{}\t{}\t{}\n",
      m[0][0], m[1][0], m[2][0], m[3][0],
      m[0][1], m[1][1], m[2][1], m[3][1],
      m[0][2], m[1][2], m[2][2], m[3][2],
      m[0][3], m[1][3], m[2][3], m[3][3]
      );
  }

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
      update_arcball(c, t);
    }else if(c.look_mode == LookMode::DIRECTION){
      update_direction(c, t);
      c.view = glm::lookAt(t._position, t._position + t._direction, c.up);
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

void Camera::update_direction(component::Camera& camera, component::Transform& t){
  auto right    = glm::cross(t._direction, camera.up);
  auto local_up = glm::cross(right, t._direction);

  t.rotate({glm::radians(_delta_cursor.y), glm::radians(_delta_cursor.x), 0});

  //t.transform = glm::rotate(t.transform, glm::radians(10.0f*_dx), local_up);
  //t.transform = glm::rotate(t.transform, glm::radians(10.0f*_dy), right);
  //t.transform = glm::translate(t.transform, _translate);

  t._position += (right*_movement_amount.x +
                   local_up*_movement_amount.y +
                   t._direction*_movement_amount.z);
  _delta_cursor = {0,0};
  _movement_amount = {0,0,0};
}

void Camera::update_arcball(component::Camera& camera, component::Transform& t){
  // radius
  t.radius = glm::max(t.radius + _movement_amount.z, 0.0f);
  // theta
  t.theta = glm::clamp<float>(t.theta - _delta_cursor.y, 0.0f, glm::pi<float>());
  // phi
  t.phi = std::fmodf(t.phi - _delta_cursor.x, 360.0f);

  // In order to get from camera to world space:
  // 1) translate radius in +z-axis,
  // 2) rotate theta about +x-axis,
  // 3) rotate phi about +z-axis,
  // 4) translate to camera target,

  auto view = glm::mat4();
  view = glm::translate(view, camera.target);
  view = glm::rotate(view, t.phi + glm::half_pi<float>(), glm::vec3(0,0,1));
  view = glm::rotate(view, t.theta, glm::vec3(1,0,0));
  view = glm::translate(view, glm::vec3(0,0,t.radius));

  // 5) the cameras position is then given as the 4th column in the matrix,
  t._position  = glm::vec3(view[3]);
  t._direction = -glm::vec3(view[2]);

  // 6) inverse of this matrix gives the view matrix.
  camera.view = glm::inverse(view);

  // Reset mouse variables.
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
