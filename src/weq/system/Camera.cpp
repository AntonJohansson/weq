#include <weq/system/Camera.hpp>
#include <weq/component/Camera.hpp>
#include <weq/component/Transform.hpp>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace weq::system{

void Camera::update(ex::EntityManager& entities,
                    ex::EventManager& events,
                    ex::TimeDelta dt){

  auto update_perspective = [dt](ex::Entity e,
                                 component::Camera& c,
                                 component::Transform& t){
    c.position = t.transform * glm::vec4(c.position, 1.0f);
    c.direction = t.transform * glm::vec4(c.direction, 0.0f);

    if(c.look_mode == LookMode::TARGET){
      c.view = glm::lookAt(c.position, c.target, c.up);
    }else if(c.look_mode == LookMode::DIRECTION){
      c.view = glm::lookAt(c.position, c.position + c.direction, c.up);
    }

    c.normal_matrix = glm::transpose(glm::inverse(c.view*t.transform));

    if(c.update_projection){
      c.projection = glm::perspective(glm::radians(c.fov), c.aspect_ratio, c.near, c.far);
    }
  };

  entities.each<component::Camera, component::Transform>(update_perspective);
}

}
