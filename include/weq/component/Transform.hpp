#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace weq::component{

// Component describing position, rotation, scale and look direction.
// Combines all these to a model matrix to be used by the renderer.
struct Transform{
  // TODO fix standard look direction and translation.
  //      make a clear distinction between translate
  //      and position.
  Transform()
    : _direction(0.0f, 0.0f, -1.0f),
      _position(0.0f, 0.0f, 0.0f),
      _scale(1.0f, 1.0f, 1.0f),
      radius(0.0f),
      theta(0.0f),
      phi(0.0f)
  {}

  // Updates the quaternion by adding a relative rotation given in
  // Euler angles. Note: this also updates the direction vector.
  void rotate(glm::vec3 euler_angles){
    glm::quat new_rot = glm::quat(euler_angles);
    _rotation = new_rot * _rotation;
    _direction = glm::normalize(new_rot * _direction);
  }

  //Updates the quaternion by adding a relative rotation around a
  // given axis. Note: also updates the direction vector.
  void rotate(float angle, glm::vec3 axis){
    glm::quat new_rot = glm::quat(angle, axis);
    _rotation = new_rot * _rotation;
    _direction = glm::normalize(new_rot * _direction);
  }

  // Combines component data to form a model matrix.
  // model = translation * rotation * scale.
  glm::mat4 model(){
    glm::mat4 model(1.0);
    model = glm::scale(model, _scale);
    model = glm::mat4_cast(_rotation)*model;
    model = glm::translate(model, _position);

    return model;
  }

  void spherical(float r, float t, float p){
    radius = r;
    theta = t;
    phi = p;
  }

  glm::vec3 _direction;
  glm::vec3 _position; // x, y, z
  glm::vec3 _scale;
  glm::quat _rotation;
  float radius;
  float theta;
  float phi;
};

} // namespace weq::component
