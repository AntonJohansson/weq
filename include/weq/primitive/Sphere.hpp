#pragma once

#include <weq/MeshData.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace weq::primitive::sphere{

static MeshData uv(float r, int parallels, int meridians){
  MeshData data(gl::format::VN);

  float pi     = glm::pi<float>();
  float two_pi = 2.0f*pi;

  data.interleaved.push_back(0.0f);
  data.interleaved.push_back(0.0f);
  data.interleaved.push_back(1.0f);

  data.interleaved.push_back(0.0f);
  data.interleaved.push_back(0.0f);
  data.interleaved.push_back(1.0f);
  for(int i = 0; i < parallels; i++){
    float theta = pi * (i+1)/parallels;

    for(int j = 0; j < meridians; j++){
      float phi = two_pi * j / meridians;

      // convert spherical -> cartesian
      data.interleaved.push_back(r*glm::sin(theta)*glm::cos(phi));
      data.interleaved.push_back(r*glm::sin(theta)*glm::sin(phi));
      data.interleaved.push_back(r*glm::cos(theta));

      data.interleaved.push_back(glm::sin(theta)*glm::cos(phi));
      data.interleaved.push_back(glm::sin(theta)*glm::sin(phi));
      data.interleaved.push_back(glm::cos(theta));
    }
  }
  data.interleaved.push_back(0.0f);
  data.interleaved.push_back(0.0f);
  data.interleaved.push_back(-1.0f);

  data.interleaved.push_back(0.0f);
  data.interleaved.push_back(0.0f);
  data.interleaved.push_back(-1.0f);

  // Setup elements

  for(int i = 0; i < meridians; ++i){
    int a = i + 1;
    int b = (i + 1) % meridians + 1;

    data.elements.push_back(0);
	data.elements.push_back(b);
	data.elements.push_back(a);
  }

  for(int j = 0; j < parallels - 2; ++j){
    int a_start = j*meridians + 1;
    int b_start = (j+1)*meridians + 1;

    for(int i = 0; i < meridians; ++i){
      int a  = a_start + i;
      int a1 = a_start + (i+1)%meridians;
      int b  = b_start + i;
      int b1 = b_start + (i+1)%meridians;

      data.elements.push_back(a);
      data.elements.push_back(a1);
      data.elements.push_back(b1);

      data.elements.push_back(a);
      data.elements.push_back(b1);
      data.elements.push_back(b);
    }
  }

  for(int i = 0; i < meridians; ++i){
    int a = i + meridians * (parallels - 2) + 1;
    int b = (i+1)%meridians + meridians * (parallels - 2) + 1;
    data.elements.push_back(data.interleaved.size()/6 - 1);
	data.elements.push_back(a);
	data.elements.push_back(b);
  }

  return data;
}

}
