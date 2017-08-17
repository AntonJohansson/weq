#pragma once

#include <glm/glm.hpp>

// Describes camera behaviour.
// TARGET    - Forces the camera to look at a set position every update.
//             Useful for orbital cameras.
//
// DIRECTION - Forces the camera to follow a given direction vector.
//             Useful for FPS camera. REQUIRES transform component.
enum class LookMode{
  TARGET,
  DIRECTION,
};

namespace component{

// Component to tag a camera entity as "Active" forcing it to be used
// by the renderer. There should only be one active camera, if multiple
// the entity that was made active the most recent is chosen.
struct ActiveCamera{};

// Component that keeps track of all the camera-relevant data.
struct Camera{
  Camera(LookMode look_mode = LookMode::DIRECTION,
         float n = 1.0f,
         float f = 100.0f,
         float fov = 45.0f,
         glm::vec3 target = {0, 0, 0})
    : look_mode(look_mode),
      _near(n),
      _far(f),
      fov(fov),
      aspect_ratio(1280.0f/720.0f),
      target(target),
      up({0, 0, 1}),
      update_projection(true){
  }

  LookMode look_mode;

  // -- Perspective
  float _near;
  float _far;
  float fov;
  float aspect_ratio;

  // -- View frustrum
  glm::vec3 target;
  glm::vec3 up;

  // -- Transformations
  glm::mat4 view;
  glm::mat4 projection;
  glm::mat4 normal_matrix;

  // viewproj = projection * view.
  glm::mat4 viewproj;

  // If true - update the current perspective, usually means that either the
  //           near field, far field, fov, or aspect ratio have changed.
  //           This tags the component to be updated by the camera system.
  //           TODO use same flags variable?
  bool update_projection;
};

} // namespace component
