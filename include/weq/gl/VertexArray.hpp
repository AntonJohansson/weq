#pragma once

#include <weq/gl/VertexFormat.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/ShaderProgram.hpp>

#include <utility>
#include <vector>
#include <memory>

namespace gl{

class VertexArray{
public:
  using BufferFormat = std::pair<VertexBuffer, VertexFormat>;

  VertexArray();
  VertexArray(std::shared_ptr<ShaderProgram> program,
              std::vector<BufferFormat> bufferformat);

  void bind_attribute(BufferFormat format);
  void bind() const;

  GLuint object() const {return _vao;}
  size_t size() const {return _size;}

private:
  std::shared_ptr<gl::ShaderProgram> _program;
  GLuint _vao;
  size_t _size;
};

};
