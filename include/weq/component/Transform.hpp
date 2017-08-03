#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace component{

// Component describing position, rotation, scale and look direction.
// Combines all these to a model matrix to be used by the renderer.
struct Transform{
  // TODO fix standard look direction and translation.
  //      make a clear distinction between translate
  //      and position.
  Transform()
    : _direction(0.0f, 0.0f, -1.0f),
      _translate(0.0f, 0.0f, 10.0f),
      _scale(1.0f, 1.0f, 1.0f){}

  // Updates the quaternion by adding a relative rotation given in
  // Euler angles. Note: this also updates the direction vector.
  void rotate(glm::vec3 euler_angles){
    glm::quat new_rot = glm::quat(euler_angles);
    _rotation = new_rot * _rotation;
    _direction = glm::normalize(new_rot * _direction);
  }

  // Combines component data to form a model matrix.
  // model = translation * rotation * scale.
  glm::mat4 model(){
    glm::mat4 model(1.0);
    model = glm::scale(model, _scale);
    model = glm::mat4_cast(_rotation)*model;
    model = glm::translate(model, _translate);

    return model;
  }

  glm::vec3 _direction;
  glm::vec3 _translate;
  glm::vec3 _scale;
  glm::quat _rotation;
};

} // namespace component
