#pragma once

#include <weq/MeshData.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace primitive::circle{

static MeshData outline(float radius, glm::vec4 color){
  MeshData data(gl::format::VC);
  float divisions = 100.0f;
  float two_pi = 2.0f*glm::pi<float>();
  float theta = two_pi/divisions;

  for(float angle = 0.0f; angle < two_pi; angle += theta){
    data.interleaved.push_back(radius*std::sin(angle));
    data.interleaved.push_back(radius*std::cos(angle));
    data.interleaved.push_back(0);

    data.interleaved.push_back(color.r);
    data.interleaved.push_back(color.g);
    data.interleaved.push_back(color.b);
    data.interleaved.push_back(color.a);
  }

  int i = 0;
  for(float angle = 0.0f; angle < two_pi; angle += theta){
    data.elements.push_back(i);
    i++;
  }

  return data;
}

} // namespace primitive::circle
