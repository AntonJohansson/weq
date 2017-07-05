#pragma once

#include <weq/gl/VertexFormat.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/Shader.hpp>

#include <utility>
#include <vector>

namespace gl{

class VertexArray{
public:
  using BufferFormat = std::pair<VertexBuffer, VertexFormat>;

  VertexArray();
  VertexArray(gl::Shader* shader,
              std::vector<BufferFormat> bufferformat);

  void bind_attribute(BufferFormat format);
  void bind() const;

  GLuint object() const {return _vao;}
  size_t size() const {return _size;}

private:
  gl::Shader* _shader;
  GLuint _vao;
  size_t _size;
};

};
