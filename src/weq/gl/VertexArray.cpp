#include <weq/gl/VertexArray.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/VertexFormat.hpp>

#include <spdlog/spdlog.h>

namespace gl{

VertexArray::VertexArray()
  : _size(0){
  glGenVertexArrays(1, &_vao);
}

VertexArray::VertexArray(std::shared_ptr<gl::ShaderProgram> program,
                         gl::VertexBuffer& vbo,
                         gl::VertexFormat& format)
  : _program(program),
    _size(0){
  glGenVertexArrays(1, &_vao);
  bind();

  bind_attribute(vbo, format);
}

void VertexArray::bind_attribute(gl::VertexBuffer& vbo, gl::VertexFormat& format){
  vbo.bind();

  for(auto& a : format.attributes){
    _program->bind_attribute(a.attribute, GLenum(a.type), a.length, format.stride, a.offset);
  }

  _size = vbo.size() / format.format_length;
}

void VertexArray::bind() const{
  glBindVertexArray(_vao);
}

} // namespace gl
