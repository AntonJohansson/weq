#pragma once

#include <glad/glad.h>

#include <memory>

namespace weq::gl{

// Defined in <weq/gl/Buffer.hpp>
template<GLenum buffer_target, typename T>
class Buffer;
using VertexBuffer = Buffer<GL_ARRAY_BUFFER, float>;

// Defined in <weq/gl/VertexFormat.hpp>
class ShaderProgram;

// Defined in <weq/gl/VertexFormat.hpp>
struct VertexFormat;

// Abstraction of an OpenGL Vertex Array that handles the
// binding between the Vertex Buffer and Shader Program.
class VertexArray{
public:
  // Creates the OpenGL handle.
  VertexArray();

  // Creates the OpenGL handle.
  // Also binds the vertex buffers and accompanied vertex format
  // with the shader program, through calling bind_attribute(...)
  VertexArray(std::shared_ptr<gl::ShaderProgram> program,
              gl::VertexBuffer& vbo,
              gl::VertexFormat& format);

  // Binds the parameter (vertex buffer and vertex format) to the shader
  // specified in the constructor.
  void bind_attribute(gl::VertexBuffer& vbo, gl::VertexFormat& format);

  // Binds for use with OpenGL functions.
  void bind() const;

  // Returns OpenGL handle.
  GLuint object() const {return _vao;}

  // Returns the amount of verticies in the vertex buffer with a
  // particular vertex format.
  size_t size() const {return _size;}

private:
  std::shared_ptr<gl::ShaderProgram> _program;
  GLuint _vao;
  size_t _size;
};

} // namespace weq::gl
