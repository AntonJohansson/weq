#include <weq/system/Camera.hpp>

#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>

#include <weq/component/Camera.hpp>
#include <weq/component/Transform.hpp>

#include <weq/event/Window.hpp>
#include <weq/event/Input.hpp>
#include <weq/event/DebugDraw.hpp>

#include <weq/vars/Vars.hpp>

//#include <spdlog/spdlog.h>
//#include <glm/gtx/rotate_vector.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtc/constants.hpp>
//#include <glm/ext.hpp>
//
//#include <algorithm>
//#include <cmath>
//
// @TODO TEMP
#include <weq/utility/GlmHelper.hpp>
#include <weq/utility/Profiler.hpp>
#include <weq/simd/matrix.hpp>
#include <weq/simd/constants.hpp>
//
//#ifdef _WIN32
//#undef near
//#undef far
//#endif

//#include <glm/gtx/quaternion.hpp>

/* TODO (bug)
   something weird happens when the camera is looking along the "up" vector,
   that results in a black image (empty view-matrix)
*/

// TODO inverted x-movement

namespace{
  bool fps_mode = false;
  Var(float, camera_speed, 0.5f);
  Var(float, camera_sensitivity, 1.0f);
}

namespace weq::system{

Camera::Camera()
	: _movement_amount(0,0,0) {
}

Camera::~Camera(){
}

void Camera::configure(EventManager& events){
  System<Camera>::configure(events);
  //events.subscribe<event::WindowUpdate>(*this);
  events.subscribe<event::ActiveWindow>(*this);
  events.subscribe<event::ActiveInput>(*this);
}

void Camera::update(EntityManager& entities,
                    EventManager& events,
                    f32 dt){
  (void)events;
  (void)dt;

  // TODO don't use glm lookat -> manual rot with quaternions.
  static auto mask = entities.generate_component_mask<component::Camera, component::Transform>();
  component::Transform* t;
  component::Camera* c;

  for(auto entity : entities.entities_with_components(mask)){
    t = entities.get<component::Transform>(entity);
    c = entities.get<component::Camera>(entity);

  //auto update_perspective = [dt, this, &events](Entity e,
  //                                             component::Camera& c,
  //                                             component::Transform& t){
    // Check aspect ratio
    if(c->inherit_aspect){
      c->aspect_ratio = _aspect_ratio;
      c->update_projection = true;
    }

    // Update look direction for both camera modes.
    if(c->look_mode == LookMode::TARGET){
      update_arcball(c, t);
    }else if(c->look_mode == LookMode::DIRECTION){
      if(!fps_mode)fps_mode = true;

      update_direction(c, t);
      c->view = glm::lookAt(t->_position, t->_position + t->_direction, c->up);
    }

    //TODO Move this? model needs to be built before render anc camera pass
    //c->normal_matrix = glm::transpose(glm::inverse(c->view*t->model()));
    //c->normal_matrix = glm::transpose(glm::inverse(glm::mat4(glm::mat3(t->model()))));

    if(c->update_projection){
      c->projection = glm::perspective(glm::radians(c->fov), c->aspect_ratio, c->near, c->far);
      c->update_projection = false;
    }
  }
    //};

  //entities.each<component::Camera, component::Transform>(update_perspective);
}

void Camera::update_direction(component::Camera* camera, component::Transform* t){
  auto right    = glm::cross(t->_direction, camera->up);
  auto local_up = glm::cross(right, t->_direction);

  _delta_cursor = -camera_sensitivity.var*_delta_cursor;
  t->rotate({glm::radians(_delta_cursor.y), 0, glm::radians(_delta_cursor.x)});

  //t->transform = glm::rotate(t->transform, glm::radians(10.0f*_dx), local_up);
  //t->transform = glm::rotate(t->transform, glm::radians(10.0f*_dy), right);
  //t->transform = glm::translate(t->transform, _translate);

  t->_position += (right*_movement_amount.x +
                   t->_direction*_movement_amount.y +
                   local_up*_movement_amount.z);
  _delta_cursor = {0,0};
  _movement_amount = {0,0,0};
}

void Camera::update_arcball(component::Camera* camera, component::Transform* t){
	auto min   = [](float a, float b){return (b < a) ? b : a;};
	auto max   = [](float a, float b){return (a < b) ? b : a;};
	auto clamp = [&max,&min](float f, float a, float b){return min(max(f,a), b);};
  // radius
  t->radius = max(t->radius + _movement_amount.z, 0.0f);
  // theta
  t->theta = clamp(t->theta - _delta_cursor.y, 0.0f, glm::pi<float>());
  // phi
  t->phi = std::fmod(t->phi - _delta_cursor.x, 2.0f*glm::pi<float>());

  // In order to get from camera to world space:
  // 1) translate radius in +z-axis,
  // 2) rotate theta about +x-axis,
  // 3) rotate phi about +z-axis,
  // 4) translate to camera target,

#define USE_SIMD 0
	//{PROFILE("arcball")
#if !USE_SIMD
		auto view = glm::mat4(1.0);
		view = glm::translate(view, camera->target);
		view = glm::rotate(view, t->phi + glm::half_pi<float>(), glm::vec3(0,0,1));
		view = glm::rotate(view, t->theta, glm::vec3(1,0,0));
		view = glm::translate(view, glm::vec3(0,0,t->radius));
		// REVERSE ORDER
		//view = glm::translate(view, glm::vec3(0,0,t->radius));
		//view = glm::rotate(view, t->theta, glm::vec3(1,0,0));
		//view = glm::rotate(view, t->phi + glm::half_pi<float>(), glm::vec3(0,0,1));
		//view = glm::translate(view, camera->target);

		// 5) the cameras position is then given as the 4th column in the matrix,
		t->_position  =  glm::vec3(view[3]);
		t->_direction = -glm::vec3(view[2]);

		// 6) inverse of this matrix gives the view matrix.
		camera->view = glm::inverse(view);
#else
		simd::v3 camera_target(camera->target.x, camera->target.y, camera->target.z);
		simd::m4 inv;
		simd::v4 pos;
		simd::v4 dir;

		using namespace simd;
		m4 view = identity_m4;
		view = mul(translate(v3(0,0,t->radius)), view);
		view = mul(axis_angle(v3(1,0,0), t->theta), view);
		view = mul(axis_angle(v3(0,0,1), t->phi + constants::half_pi), view);
		view = mul(translate(camera_target), view);

		pos = view.w;
		dir = -view.z;
		inv = transform_inverse_no_scale(view);

		t->_position  = glm::vec3(get_x(pos), get_y(pos), get_z(pos));
		t->_direction = glm::vec3(get_x(dir), get_y(dir), get_z(dir));
		camera->view = glm::mat4(
			glm::vec4(get_x(inv.x), get_y(inv.x), get_z(inv.x), get_w(inv.x)),
			glm::vec4(get_x(inv.y), get_y(inv.y), get_z(inv.y), get_w(inv.y)),
			glm::vec4(get_x(inv.z), get_y(inv.z), get_z(inv.z), get_w(inv.z)),
			glm::vec4(get_x(inv.w), get_y(inv.w), get_z(inv.w), get_w(inv.w))
			);

#endif
	//}


  // Reset mouse variables.
  _delta_cursor = {0,0};
  _movement_amount = {0,0,0};
}

void Camera::receive(event::ActiveWindow& event){
  _aspect_ratio = event.window->aspect_ratio();
}

void Camera::receive(event::ActiveInput& event){
	// TEMP

	//spdlog::get("console")->info("actions:");
	//for(auto key : event.actions){
	//	spdlog::get("console")->info(key);
	//}
	//spdlog::get("console")->info("states:");
	//for(auto key : event.states){
	//	spdlog::get("console")->info(key);
	//}
	//spdlog::get("console")->info("ranges:");
	//for(auto key : event.ranges){
	//	spdlog::get("console")->info(key);
	//}
  // Only update mouse camera direction when mouse is down
  if(event.has(InputState::CURSOR_DOWN)){
    // Update mouse delta position
	if(!event.has(InputState::CURSOR_DOWN_IN_UI) && event.has(InputRange::CURSOR_DX) && event.has(InputRange::CURSOR_DY)){
		// TODO: DOES THIS COUNT AS SENSITIVITY?
		_delta_cursor.x = 10.0f * event.range_values.at(static_cast<int>(InputRange::CURSOR_DX));
		_delta_cursor.y = 10.0f * event.range_values.at(static_cast<int>(InputRange::CURSOR_DY));
    }
  }

  // Update camera distance with scroll
  if(event.has(InputRange::CURSOR_SCROLL_X) && event.has(InputRange::CURSOR_SCROLL_Y)){
    //double x = event.ranges.at(InputRange::CURSOR_SCROLL_X);
    _movement_amount.z = -0.5*event.range_values.at(static_cast<int>(InputRange::CURSOR_SCROLL_Y));
  }

  // First person
  // Update camera movement vector
  if(fps_mode){
    if(event.has(InputState::MOVE_LEFT)){
      _movement_amount.x = -camera_speed.var;
    }if(event.has(InputState::MOVE_RIGHT)){
      _movement_amount.x = +camera_speed.var;
    }if(event.has(InputState::MOVE_FORWARD)){
      _movement_amount.y = +camera_speed.var;
    }if(event.has(InputState::MOVE_BACK)){
      _movement_amount.y = -camera_speed.var;
    }if(event.has(InputState::MOVE_UP)){
      _movement_amount.z = +camera_speed.var;
    }if(event.has(InputState::MOVE_DOWN)){
      _movement_amount.z = -camera_speed.var;
    }
  }
}

}
