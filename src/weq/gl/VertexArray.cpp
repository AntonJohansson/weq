#include <weq/gl/VertexArray.hpp>
#include <glad/glad.h>
#include <iostream>

namespace gl{

VertexArray::VertexArray()
  : _size(0)
{
  glGenVertexArrays(1, &_vao);
}

VertexArray::VertexArray(gl::Shader* shader,
                         std::vector<BufferFormat> bufferformat)
  : _shader(shader)
  , _size(0)
{
  glGenVertexArrays(1, &_vao);
  bind();

  for(auto& bf : bufferformat){
    bind_attribute(bf);
  }
}

void VertexArray::bind_attribute(BufferFormat bufferformat){
  auto& buffer = bufferformat.first;
  auto& format = bufferformat.second;

  buffer.bind();

  for(auto& v : format.components){
    _shader->bind_attribute(v.attribute, v.type, v.size, format.stride, v.offset);
  }

  auto size = buffer.size() / format.vertex_size;

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
