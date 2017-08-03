#include <weq/gl/VertexArray.hpp>
#include <glad/glad.h>
#include <iostream>

namespace gl{

VertexArray::VertexArray()
  : _size(0){
  glGenVertexArrays(1, &_vao);
}

VertexArray::VertexArray(std::shared_ptr<ShaderProgram> program,
                         gl::VertexBuffer& vbo,
                         gl::VertexFormat format)
  : _program(program),
    _size(0){
  glGenVertexArrays(1, &_vao);
  bind();

  bind_attribute(vbo, format);
}

void VertexArray::bind_attribute(gl::VertexBuffer& vbo, gl::VertexFormat format){
  vbo.bind();

  for(auto& a : format.attributes){
    _program->bind_attribute(a.attribute, GLenum(a.type), a.length, format.stride, a.offset);
  }

  auto size = vbo.size() / format.format_length;

  if(_size != 0 && size != _size){
    std::cerr << "Format mismatch" << std::endl;
  }else{
    _size = size;
  }
}

void VertexArray::bind() const{
  glBindVertexArray(_vao);
}

};
