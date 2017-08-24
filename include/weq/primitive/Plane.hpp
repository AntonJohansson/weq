#pragma once

#include <weq/MeshData.hpp>
#include <spdlog/spdlog.h>

namespace primitive::plane{

// Generates meshdata for a point plane of size w * h with a fixed gridsize.
// Data generated according to passed in format.
static MeshData points(int w, int h, float gridsize, gl::VertexFormat format){
  MeshData data(format);

  for(int x = 0; x <= w; x++){
    for(int y = 0; y <= h; y++){
      // vertices
      if(auto v = data.format.has("position")){
        data.interleaved.push_back(x * gridsize);
        data.interleaved.push_back(y * gridsize);
        data.interleaved.push_back(x * gridsize);
      }

      // normals
      if(auto n = data.format.has("normal")){
        data.interleaved.push_back(0.0f);
        data.interleaved.push_back(0.0f);
        data.interleaved.push_back(1.0f);
      }

      // color
      if(auto c = data.format.has("color")){
        data.interleaved.push_back(1.0f);
        data.interleaved.push_back(1.0f);
        data.interleaved.push_back(1.0f);
        data.interleaved.push_back(1.0f);
      }

      // texcoords
      if(auto t = data.format.has("texcoord")){
        data.interleaved.push_back(static_cast<float>(x)/static_cast<float>(w));
        data.interleaved.push_back(static_cast<float>(y)/static_cast<float>(h));
      }

      data.elements.push_back(x*(w+1) + y);
    }
  }

  return data;
}

// Generates meshdata for a solid plane (consisting of triangles) of size w * h
// with a fixed gridsize.
// Data generated according to passed in format.
static MeshData solid(int w, int h, float gridsize, gl::VertexFormat format){
  MeshData data(format);

  // Check format
  bool has_position = data.format.has("position");
  bool has_normal   = data.format.has("normal");
  bool has_color    = data.format.has("color");
  bool has_texcoord = data.format.has("texcoord");

  // Precompute memory
  // w+1 and h+1 is used since the plane is based of quads.
  data.interleaved.reserve(format.format_length*(h+1)*(h+1)); // TODO is this calculation correct?
  // w and h is used since indices do not need to be calculated for the last row.
  data.elements.reserve(6*w*h);

  for(int x = 0; x <= w; x++){
    for(int y = 0; y <= h; y++){
      // vertices
      if(has_position){
        data.interleaved.push_back(x * gridsize);
        data.interleaved.push_back(y * gridsize);
        data.interleaved.push_back(0.0f);
      }

      // normals
      if(has_normal){
        data.interleaved.push_back(0.0f);
        data.interleaved.push_back(0.0f);
        data.interleaved.push_back(1.0f);
      }

      // color
      if(has_color){
        data.interleaved.push_back(1.0f);
        data.interleaved.push_back(1.0f);
        data.interleaved.push_back(1.0f);
        data.interleaved.push_back(1.0f);
      }

      // texcoords
      if(has_texcoord){
        data.interleaved.push_back(static_cast<float>(x)/static_cast<float>(w));
        data.interleaved.push_back(static_cast<float>(y)/static_cast<float>(h));
      }

      // indices
      if(x < w && y < h){
        data.elements.push_back(x*(w+1)     + y);     // bottom left
        data.elements.push_back(x*(w+1)     + (y+1)); // top left
        data.elements.push_back((x+1)*(w+1) + (y+1)); // top right

        data.elements.push_back((x+1)*(w+1) + (y+1)); // top right
        data.elements.push_back((x+1)*(w+1) + y);     // bottom right
        data.elements.push_back(x*(w+1)     + y);     // bottom left
      }
    }
  }

  return data;
}

} // namespace primitive::plane
