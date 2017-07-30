#pragma once

#include <weq/MeshData.hpp>

namespace primitive::plane{

MeshData points(int w, int h, float gridsize, gl::VertexFormat format){
  MeshData data(format);

  for(int x = 0; x <= w; x++){
    for(int y = 0; y <= h; y++){
      // vertices
      if(auto v = data.format.has("position")){
        data.interleaved.push_back(x * gridsize);
        data.interleaved.push_back(y * gridsize);
        data.interleaved.push_back(x*gridsize);
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

MeshData solid(int w, int h, float gridsize, gl::VertexFormat format){
  MeshData data(format);

  for(int x = 0; x <= w; x++){
    for(int y = 0; y <= h; y++){
      // vertices
      if(auto v = data.format.has("position")){
        data.interleaved.push_back(x * gridsize);
        data.interleaved.push_back(y * gridsize);
        data.interleaved.push_back(0.0f);
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

};
