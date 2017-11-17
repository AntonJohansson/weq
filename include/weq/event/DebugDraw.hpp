#pragma once

#include <glm/glm.hpp>

namespace event{

enum class DebugMode{
  PERSISTENT,
  FRAME,
};

enum class DrawType{
  VECTOR
};

struct DebugDraw{
  DrawType type;
  DebugMode mode;
  glm::vec3 vec;
  glm::vec3 pos;
  glm::vec4 color;

  DebugDraw(DrawType t, DebugMode m, glm::vec3 v, glm::vec3 p, glm::vec4 c)
    : type(t),
      mode(m),
      vec(v),
      pos(p),
      color(c){}
};

}
