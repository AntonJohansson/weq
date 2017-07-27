#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace component{

struct Transform{
  Transform()
    : _scale(1.0f, 1.0f, 1.0f),
      _direction(0.0f, 0.0f, -1.0f),
      _translate(0.0f, 0.0f, 10.0f){}

  void rotate(glm::vec3 euler_angles){
    glm::quat new_rot = glm::quat(euler_angles);
    _rotation = new_rot * _rotation;
    _direction = glm::normalize(new_rot * _direction);
  }

  glm::mat4 model(){
    glm::mat4 model(1.0);
    model = glm::scale(model, _scale);
    model = glm::mat4_cast(_rotation)*model;
    model = glm::translate(model, _translate);

    return model;
  }

  glm::vec3 _scale;
  glm::vec3 _direction;
  glm::vec3 _translate;
  glm::quat _rotation;
  //glm::mat4 _transform;
};

}
