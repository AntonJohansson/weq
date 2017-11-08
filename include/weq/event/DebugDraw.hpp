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
};

}
