#pragma once

#include <glm/glm.hpp>

namespace event{

enum class DrawType{
  VECTOR
};

struct DebugDraw{
  DrawType type;
  glm::vec3 vec;
  glm::vec3 pos;
  glm::vec4 color;

  DebugDraw(DrawType t, glm::vec3 v, glm::vec3 p, glm::vec4 c)
    : type(t),
      vec(v),
      pos(p),
      color(c){}
};

}
