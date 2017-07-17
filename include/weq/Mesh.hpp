#pragma once

#include <weq/MeshData.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/VertexArray.hpp>
#include <cmath>

#include <glm/glm.hpp>

#include <iostream>
#include <vector>

class Mesh{
public:
  Mesh(MeshData data, gl::Shader* shader)
    : _data(data){
    if(_data.has_vertices() && _data.has_vertices() && _data.has_normals()){
      for(int i = 0; i < _data.vertices.size(); i+=3){
        _interleaved_data.push_back(_data.vertices[i + 0]);
        _interleaved_data.push_back(_data.vertices[i + 1]);
        _interleaved_data.push_back(_data.vertices[i + 2]);

        _interleaved_data.push_back(_data.normals[i + 0]);
        _interleaved_data.push_back(_data.normals[i + 1]);
        _interleaved_data.push_back(_data.normals[i + 2]);

        _interleaved_data.push_back(_data.color[i + 0]);
        _interleaved_data.push_back(_data.color[i + 1]);
        _interleaved_data.push_back(_data.color[i + 2]);
        _interleaved_data.push_back(_data.color[i + 3]);
      }

      _vbo = {gl::Usage::STREAM, _interleaved_data};
      _mapped_region = _vbo.map(gl::Access::READ_WRITE);

      _vao = {shader, {{_vbo, gl::formats::VNC}}};

      _ebo = {gl::Usage::STATIC, _data.elements};
    }
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
  std::vector<float> _interleaved_data;
  MeshData _data;
  gl::VertexArray  _vao;
  gl::VertexBuffer _vbo;
  gl::ElementBuffer _ebo;
  float* _mapped_region;
};
