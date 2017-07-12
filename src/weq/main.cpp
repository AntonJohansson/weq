#include <weq/Window.hpp>
#include <weq/Engine.hpp>
#include <weq/Node.hpp>
#include <weq/gl/VertexArray.hpp>
#include <weq/FlatBuffer.hpp>
#include <weq/Camera.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/Wave.hpp>
#include <weq/primitive/Plane.hpp>
#include <weq/Mesh.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <deque>
#include <algorithm>
#include <cmath>
#include <iostream>

//    _delta.fill(0);
//    _height_field.fill(0);
//
//    for(int j = 0; j <= _height; j++){
//      _height_field(_width/2, j) = 3.0f;
//    }
//    _c = 0.2f;
//    //_height_field(_width/2, _height/2) = 5.0f;
//  }
//
//  ~Grid(){
//  }
//
//  float& region_z(int i, int j, int offset = 2){
//    assert((i*(_width + 1) + j)*3 + offset < vertices.size());
//    return _mapped_vertices[(i*(_width + 1) + j) * 3 + offset];
//  }
//
//  void update(float dt) override {
//    static float gridsize2 = _gridsize*_gridsize;
//    static float r = _c*_c*dt*dt/gridsize2;
//    static float c2 = _c*_c;
//
//    for(int i = 0; i <= _width; i++){
//      for(int j = 0; j <= _height; j++){
//        auto l = _height_field(std::max(i-1, 0), j);
//        auto r = _height_field(std::min(i+1, (int)_width), j);
//        auto n = _height_field(i, std::min(j+1, (int)_height));
//        auto s = _height_field(i, std::max(j-1, 0));
//
//        float f = c2*(l + r + n + s - 4.0f*_height_field(i,j))/gridsize2;
//        _delta(i,j) += f*dt;
//        _delta(i,j) *= 0.997f;
//
//        region_z(i,j) = _height_field(i,j) + _delta(i,j)*dt;
//        //float col = region_z(i,j);
//        //color(i,j, col, col, col);
//      }
//    }
//
//    for(int i = 0; i <= _width; i++){
//      for(int j = 0; j <= _height; j++){
//        _height_field(i,j) = region_z(i,j);
//      }
//    }
//  }
//};

int main(){
  weq::engine::initialize();

  float size = 100.0f;

  auto shader = new gl::Shader("vertex.vert", "fragment.frag");
  auto mesh_data = primitive::plane::solid(size, size, 5.0f/size);
  auto mesh = new Mesh(mesh_data, shader);

  //_model = glm::translate(_model, {-w*gridsize/2.0f, -h*gridsize/2.0f, 0.0f});

  auto wave = weq::engine::create_entity();
  //wave.assign<component::Wave>(size, size, 5.0f/size, 0.2f);
  wave.assign<component::Transform>();
  wave.assign<component::Renderable>(mesh, shader);

  weq::engine::main_loop();

  return 0;
}
