#pragma once

#include <weq/MeshData.hpp>

namespace primitive::plane{

MeshData solid(int w, int h, float gridsize){
  MeshData data;
  for(int x = 0; x <= w; x++){
    for(int y = 0; y <= h; y++){
      data.vertices.push_back(x * gridsize); // x
      data.vertices.push_back(y * gridsize); // y
      data.vertices.push_back(0.0f);         // y

      data.color.push_back(1.0f);
      data.color.push_back(1.0f);
      data.color.push_back(1.0f);
      data.color.push_back(1.0f);

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
