#pragma once

#include <weq/MeshData.hpp>
//#include <glm/glm.hpp>

namespace weq::primitive::vector{

static MeshData solid(glm::vec3 vec, glm::vec4 color){
  MeshData data(gl::format::VC);

  data.interleaved.push_back(0);
  data.interleaved.push_back(0);
  data.interleaved.push_back(0);

  data.interleaved.push_back(color.r);
  data.interleaved.push_back(color.g);
  data.interleaved.push_back(color.b);
  data.interleaved.push_back(color.a);

  data.interleaved.push_back(vec.x);
  data.interleaved.push_back(vec.y);
  data.interleaved.push_back(vec.z);

  data.interleaved.push_back(color.r);
  data.interleaved.push_back(color.g);
  data.interleaved.push_back(color.b);
  data.interleaved.push_back(color.a);

  data.elements.push_back(0);
  data.elements.push_back(1);

  return data;
}

} // namespace weq::primitive::vector
