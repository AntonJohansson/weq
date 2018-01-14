#pragma once

#include <weq/MeshData.hpp>
#include <glm/glm.hpp>

namespace primitive::ray{

static MeshData solid(glm::vec3 direction, glm::vec4 color){
  MeshData data(gl::format::VC);
  float far_plane = 10.0f;

  glm::vec3 far_vector = far_plane*direction;

  data.interleaved.push_back(0);
  data.interleaved.push_back(0);
  data.interleaved.push_back(0);

  data.interleaved.push_back(color.r);
  data.interleaved.push_back(color.g);
  data.interleaved.push_back(color.b);
  data.interleaved.push_back(color.a);

  data.interleaved.push_back(far_vector.x);
  data.interleaved.push_back(far_vector.y);
  data.interleaved.push_back(far_vector.z);

  data.interleaved.push_back(color.r);
  data.interleaved.push_back(color.g);
  data.interleaved.push_back(color.b);
  data.interleaved.push_back(color.a);

  data.elements.push_back(0);
  data.elements.push_back(1);

  return data;
}

} // namespace primitive::ray
