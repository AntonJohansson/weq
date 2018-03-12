#pragma once

#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Framebuffer.hpp>
#include <weq/gl/Texture.hpp>
#include <memory>

namespace weq::component{

// Component describing all the relevant data for performing
// FDM Wave. Eq. calculations on the GPU through FBOs.
struct WaveGPU{
  // Parameters:
  //   w - width of simulation grid,
  //   h - height of simulation grid,
  //   gridsize - distance between two following grid points,
  //   c - Wave velocity (unit?) used to calculate the constant
  //         r = c^2/gridsize^2,
  //       which is used to speed up force calculations.
  // change name of wavespeed param -> members _name.
  WaveGPU(unsigned int w, unsigned int h,
          float gridsz, float wavespeed)
    : width(w),
      height(h),
      gridsize(gridsz),
      c(wavespeed),
      r(wavespeed*wavespeed/(gridsz*gridsz)),
      vel_fbo(   w, h, GL_R32F, GL_RED, GL_FLOAT),
      height_fbo(w, h, GL_R32F, GL_RED, GL_FLOAT),
      edge_fbo(  w, 4, GL_R32F, GL_RED, GL_FLOAT),
      normal_fbo(w, h){
  }

  void set_c(float wavespeed){
    c = wavespeed;
    r = (c*c)/(gridsize*gridsize);
  }

  unsigned int width;
  unsigned int height;

  float gridsize;
  float c;
  float r;

  gl::Framebuffer vel_fbo;
  gl::Framebuffer height_fbo;
  gl::Framebuffer edge_fbo;
  gl::Framebuffer normal_fbo;
};

} // namespace weq::component
