#pragma once

#include <weq/MeshData.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/VertexArray.hpp>
#include <cmath>

#include <glm/glm.hpp>

#include <vector>

class Mesh{
public:
  Mesh(MeshData data, std::shared_ptr<gl::ShaderProgram> program)
    : _data(data){
    _vbo = {gl::Usage::STREAM, _data.interleaved};
    _mapped_region = _vbo.map(gl::Access::READ_WRITE);

    _vao = {program, {{_vbo, gl::formats::VNCT}}};

    _ebo = {gl::Usage::STATIC, _data.elements};
  }

  ~Mesh(){
    _vbo.unmap();
  }

  float& vertex_x(int index){return _mapped_region[10*index + 0];}
  float& vertex_y(int index){return _mapped_region[10*index + 1];}
  float& vertex_z(int index){return _mapped_region[10*index + 2];}
  float& normal_x(int index){return _mapped_region[10*index + 3];}
  float& normal_y(int index){return _mapped_region[10*index + 4];}
  float& normal_z(int index){return _mapped_region[10*index + 5];}
  float& color_r(int index){return  _mapped_region[10*index + 6];}
  float& color_g(int index){return  _mapped_region[10*index + 7];}
  float& color_b(int index){return  _mapped_region[10*index + 8];}
  float& color_a(int index){return  _mapped_region[10*index + 9];}

  const gl::VertexArray& vao() const {return _vao;}


  void bind_ebo(){_ebo.bind();}
  size_t indices(){return _ebo.size();}

private:
  MeshData _data;
  gl::VertexArray  _vao;
  gl::VertexBuffer _vbo;
  gl::ElementBuffer _ebo;
  float* _mapped_region;
};
