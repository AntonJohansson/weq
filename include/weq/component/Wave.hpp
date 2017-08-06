#pragma once

#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Framebuffer.hpp>
#include <weq/FlatBuffer.hpp>
#include <memory>

namespace component{

// Component describing all the relevant data for performing
// FDM Wave. Eq. calculations on the GPU through FBOs.
struct WaveGPU{
  using ProgramPtr = std::shared_ptr<gl::ShaderProgram>;

  // Parameters:
  //   w - width of simulation grid,
  //   h - height of simulation grid,
  //   gridsize - distance between two following grid points,
  //   c - Wave velocity (unit?) used to calculate the constant
  //         r = c^2/gridsize^2,
  //       which is used to speed up force calculations.
  WaveGPU(unsigned int w, unsigned int h,
          float gridsize, float c,
          ProgramPtr force)
    : width(w),
      height(h),
      r(c*c/(gridsize*gridsize)),
      force_fbo(w, h),
      force_shader(force){
  }

  unsigned int width;
  unsigned int height;

  float r;

  gl::Framebuffer force_fbo;
  std::shared_ptr<gl::ShaderProgram> force_shader;
};

// Component describing all the relevant data for performing
// FDM Wave. Eq. calculations on the CPU.
// TODO remove?
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

} // namespace component
