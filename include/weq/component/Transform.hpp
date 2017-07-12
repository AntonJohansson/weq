#pragma once

#include <glm/glm.hpp>

namespace component{

struct Transform{
  Transform()
    : transform(1.0){}

  glm::mat4 transform;
};

}
