#pragma once

#include <glm/glm.hpp>

namespace weq::event{

struct DebugDrawBase{
  glm::vec3 position;
  glm::vec4 color;
  float duration;
  bool has_duration;

  DebugDrawBase(glm::vec3 p, glm::vec4 c, float d = -1.0f)
    : position(p),
      color(c),
      duration(d),
      has_duration(d >= 0.0f) {}
};

struct DebugVector : DebugDrawBase{
  glm::vec3 vector;

  DebugVector(glm::vec3 v, glm::vec3 p, glm::vec4 c, float t = -1.0f)
    : DebugDrawBase(p, c, t),
      vector(v) {}
};

struct DebugRay : DebugDrawBase{
  glm::vec3 direction;

  DebugRay(glm::vec3 d, glm::vec3 p, glm::vec4 c, float t = -1.0f)
    : DebugDrawBase(p, c, t),
      direction(d) {}
};

struct DebugCircle : DebugDrawBase{
  float radius;

  DebugCircle(float r, glm::vec3 p, glm::vec4 c, float t = -1.0f)
    : DebugDrawBase(p, c, t),
      radius(r) {}
};

} // namespace weq::event
