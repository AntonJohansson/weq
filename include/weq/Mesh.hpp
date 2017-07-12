#pragma once

#include <weq/MeshData.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/VertexArray.hpp>
#include <vector>


class Mesh{
public:
  Mesh(MeshData data, gl::Shader* shader)
    : _data(data){
    if(_data.has_vertices() && _data.has_vertices()){
      _vertex = {gl::Usage::STATIC, _data.vertices};
      _mapped_region = _vertex.map(gl::Access::READ_WRITE);

      _color = {gl::Usage::STATIC, _data.color};

      _vaos.push_back({shader, {
            {_vertex, gl::formats::V},
            {_color, gl::formats::C}
          }});

      _ebo = {gl::Usage::STATIC, _data.elements};
    }
  }

  ~Mesh(){
    _vertex.unmap();
  }

  float& vertex(int index){return _mapped_region[index];}
  const std::vector<gl::VertexArray>& vaos() const {return _vaos;}


  void bind_ebo(){_ebo.bind();}
  size_t indices(){return _ebo.size();}

private:
  MeshData _data;
  std::vector<gl::VertexArray> _vaos;
  gl::VertexBuffer _vertex;
  gl::VertexBuffer _color;
  gl::ElementBuffer _ebo;
  float* _mapped_region;
};
