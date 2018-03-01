#pragma once

#include <weq/MeshData.hpp>

namespace weq::primitive::cube{

static MeshData solid(float side){
  MeshData data(gl::format::VN);

  float l = side/2.0f;

  data.interleaved = {
    // front
     l, -l,  l,     // top left
     0, -1,  0,
    -l, -l,  l,     // top right
     0, -1,  0,
    -l, -l, -l,     // bottom right
     0, -1,  0,
     l, -l, -l,     // bottom left
     0, -1,  0,
    // back
    -l,  l,  l,     // top left
     0,  1,  0,
     l,  l,  l,     // top right
     0,  1,  0,
     l,  l, -l,     // bottom right
     0,  1,  0,
    -l,  l, -l,     // bottom left
     0,  1,  0,
    // right
    -l, -l,  l,     // top left
     1,  0,  0,
    -l,  l,  l,     // top right
     1,  0,  0,
    -l,  l, -l,     // bottom right
     1,  0,  0,
    -l, -l, -l,     // bottom left
     1,  0,  0,
    // left
     l,  l,  l,     // top left
    -1,  0,  0,
     l, -l,  l,     // top right
    -1,  0,  0,
     l, -l, -l,     // bottom right
    -1,  0,  0,
     l,  l, -l,     // bottom left
    -1,  0,  0,
    // top
     l,  l,  l,     // top left
     0,  0,  1,
    -l,  l,  l,     // top right
     0,  0,  1,
    -l, -l,  l,     // bottom right
     0,  0,  1,
     l, -l,  l,     // bottom left
     0,  0,  1,
    // bottom
    -l,  l, -l,     // top left
     0,  0, -1,
     l,  l, -l,     // top right
     0,  0, -1,
     l, -l, -l,     // bottom right
     0,  0, -1,
    -l, -l, -l,     // bottom left
     0,  0, -1,
  };

  for(int i = 0; i < 6; i++){
    // 0 - top left
    // 1 - top right
    // 2 - bottom left
    // 3 - bottom right
    data.elements.push_back(i*4 + 0);
    data.elements.push_back(i*4 + 3);
    data.elements.push_back(i*4 + 2);

    data.elements.push_back(i*4 + 0);
    data.elements.push_back(i*4 + 2);
    data.elements.push_back(i*4 + 1);
  }

  return data;
}


static MeshData inwards_facing(float side){
  MeshData data(gl::format::V);
  float l = side/2.0f;

  data.interleaved = {
    // front
     l, -l,  l,     // top left
    -l, -l,  l,     // top right
    -l, -l, -l,     // bottom right
     l, -l, -l,     // bottom left
     // back
    -l,  l,  l,     // top left
     l,  l,  l,     // top right
     l,  l, -l,     // bottom right
    -l,  l, -l,     // bottom left

     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,
     //0,0,0,0,



    // right
    -l, -l,  l,     // top left
    -l,  l,  l,     // top right
    -l,  l, -l,     // bottom right
    -l, -l, -l,     // bottom left
    // left
     l,  l,  l,     // top left
     l, -l,  l,     // top right
     l, -l, -l,     // bottom right
     l,  l, -l,     // bottom left
    // top
     l,  l,  l,     // top left
    -l,  l,  l,     // top right
    -l, -l,  l,     // bottom right
     l, -l,  l,     // bottom left
    // bottom
    -l,  l, -l,     // top left
     l,  l, -l,     // top right
     l, -l, -l,     // bottom right
    -l, -l, -l,     // bottom left
  };

  for(int i = 0; i < 6; i++){
    // 0 - top left
    // 1 - top right
    // 2 - bottom left
    // 3 - bottom right
    data.elements.push_back(i*4 + 0);
    data.elements.push_back(i*4 + 1);
    data.elements.push_back(i*4 + 2);

    data.elements.push_back(i*4 + 2);
    data.elements.push_back(i*4 + 3);
    data.elements.push_back(i*4 + 0);
  }

  return data;
}

} // namespace weq::primitive::circle
