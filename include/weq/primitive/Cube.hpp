#pragma once

#include <weq/MeshData.hpp>

namespace weq::primitive::cube{

static MeshData solid(float side){
  MeshData data(gl::format::V);
  float l = side/2.0f;

  data.interleaved = {
    // front
    -l, -l,  l,     // top left
     l, -l,  l,     // top right
     l, -l, -l,     // bottom right
    -l, -l, -l,     // bottom left
    // back
     l,  l, -l,     // top left
    -l,  l, -l,     // top right
    -l, -l, -l,     // bottom left
     l, -l, -l,     // bottom right
    // right
     l,  l,  l,     // top left
     l,  l, -l,     // top right
     l, -l,  l,     // bottom left
     l, -l, -l,     // bottom right
    // left
    -l,  l, -l,     // top left
    -l,  l,  l,     // top right
    -l, -l, -l,     // bottom left
    -l, -l,  l,     // bottom right
    // top
    -l,  l, -l,     // top left
     l,  l, -l,     // top right
    -l,  l,  l,     // bottom left
     l,  l,  l,     // bottom right
    // bottom
    -l, -l,  l,     // top left
     l  -l,  l,     // top right
    -l, -l, -l,     // bottom left
     l, -l, -l,     // bottom right
  };

  data.elements = {
    0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,

    8, 9, 10,
    10, 11, 8,

    12, 13, 14,
    14, 15, 12,

    16, 17, 18,
    18, 19, 16,

    20, 21, 22,
    22, 23, 20
  };

  return data;
}

} // namespace weq::primitive::circle
