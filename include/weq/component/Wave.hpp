#pragma once

#include <weq/FlatBuffer.hpp>

namespace component{

struct Wave{
  Wave(unsigned int w, unsigned int h, float gridsize, float c)
    : width(w),
      height(h),
      gridsize(gridsize),
      c(c),
      delta(w + 1, h + 1){
    delta.fill(0);
  }

  unsigned int width;
  unsigned int height;
  float gridsize;
  float c;
  FlatBuffer<float> delta;
};

}
