#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace component{

struct Transform{
  Transform()
    : transform(1.0){}

  void translate(glm::vec3 vec){
    transform = glm::translate(transform, vec);
  }

  glm::mat4 transform;
};

}
