#pragma once

#include <weq/MeshData.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/VertexArray.hpp>
#include <weq/gl/DrawModes.hpp>

#include <glm/glm.hpp>

#include <cmath>
#include <vector>

class Mesh{
public:
  Mesh(MeshData data, gl::DrawMode mode)
    : _data(data),
      _draw_mode(mode){

    _vbo = {gl::Usage::STREAM_DRAW, _data.interleaved};
    _mapped_region = _vbo.map(gl::Access::READ_WRITE);

    //_vao = {program, {{_vbo, _data.format}}};

    _ebo = {gl::Usage::STATIC_DRAW, _data.elements};
  }

  ~Mesh(){
    _vbo.unmap();
  }

  float& vertex_x(int index){return _mapped_region[12*index + 0];}
  float& vertex_y(int index){return _mapped_region[12*index + 1];}
  float& vertex_z(int index){return _mapped_region[12*index + 2];}
  float& normal_x(int index){return _mapped_region[12*index + 3];}
  float& normal_y(int index){return _mapped_region[12*index + 4];}
  float& normal_z(int index){return _mapped_region[12*index + 5];}
  float& color_r(int index){return  _mapped_region[12*index + 6];}
  float& color_g(int index){return  _mapped_region[12*index + 7];}
  float& color_b(int index){return  _mapped_region[12*index + 8];}
  float& color_a(int index){return  _mapped_region[12*index + 9];}

  gl::VertexArray vao(std::shared_ptr<gl::ShaderProgram> program) const {
    return {program, {{_vbo, _data.format}}};
  }
  const gl::ElementBuffer& ebo() const {return _ebo;}

  gl::DrawMode draw_mode(){return _draw_mode;}

private:
  MeshData _data;
  gl::DrawMode _draw_mode;
  gl::VertexBuffer _vbo;
  gl::ElementBuffer _ebo;
  float* _mapped_region;
};
