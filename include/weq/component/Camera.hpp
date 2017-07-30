#pragma once

#include <glm/glm.hpp>

enum class LookMode{
  TARGET,
  DIRECTION,
};

namespace component{

struct ActiveCamera{};

struct Camera{
  Camera(LookMode look_mode = LookMode::DIRECTION,
         float near = 1.0f,
         float far = 100.0f,
         float fov = 45.0f,
         glm::vec3 target = {0, 0, 0})
    : look_mode(look_mode),
      near(near),
      far(far),
      fov(fov),
      aspect_ratio(1280.0f/800.0f),
      target(target),
      up({0, 0, 1}),
      update_projection(true){
  }

  LookMode look_mode;
  float near;
  float far;
  float fov;
  float aspect_ratio;
  glm::vec3 target;
  glm::vec3 up;
  glm::mat4 view;
  glm::mat4 projection;
  glm::mat4 normal_matrix;

  glm::mat4 viewproj;

  bool update_projection;
};

}
