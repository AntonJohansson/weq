#pragma once

#include <weq/FlatBuffer.hpp>

namespace component{

struct Wave{
  Wave(int w, int h, float gridsize, float c)
    : width(w),
      height(h),
      gridsize(gridsize),
      c(c),
      r(c*c/(gridsize*gridsize)),
      delta(w + 1, h + 1),
      height_field(w + 1, h + 1){
    delta.fill(0);
    height_field.fill(0);
  }

  unsigned int to_index(int i, int j){
    return (i*(width + 1) + j);
  }

  int width;
  int height;
  float gridsize;
  float c;
  float r;
  FlatBuffer<float> delta;
  FlatBuffer<float> height_field;
  //FlatBuffer<float> edge;
};

}
