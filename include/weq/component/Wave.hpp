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

    l_edge = new float[height+1];
    r_edge = new float[height+1];
    n_edge = new float[width+1];
    s_edge = new float[width+1];

    for(int j = 0; j <= height; j++){
      l_edge[j] = 0.0f;
      r_edge[j] = 0.0f;
    }
    for(int i = 0; i <= width; i++){
      n_edge[i] = 0.0f;
      s_edge[i] = 0.0f;
    }

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
  float* l_edge;
  float* r_edge;
  float* n_edge;
  float* s_edge;
};

}
