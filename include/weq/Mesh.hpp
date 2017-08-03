#pragma once

#include <weq/MeshData.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/VertexArray.hpp>
#include <weq/gl/DrawModes.hpp>

#include <glm/glm.hpp>

#include <cmath>
#include <vector>

// Class that takes mesh data and generates a VBO and EBO that can be
// used for rendering.
class Mesh{
public:
  // Convertes MeshData to a VBO and EBO. The OpenGL draw mode is stored
  // for later use. Currently maps VBO-region to memory for easy
  // modification.
  Mesh(MeshData data, gl::DrawMode mode)
    : _data(data),
      _draw_mode(mode){

    _vbo = {gl::Usage::STREAM_DRAW, _data.interleaved};
    _mapped_region = _vbo.map(gl::Access::READ_WRITE);

    //_vao = {program, {{_vbo, _data.format}}};

    _ebo = {gl::Usage::STATIC_DRAW, _data.elements};
  }

  // Unmaps the VBO from memory and automatically calls
  // destructors for the VBO, EBO.
  ~Mesh(){
    _vbo.unmap();
  }

  // TODO remove
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

  // Generates a VAO-binding for a given shader program.
  gl::VertexArray vao(std::shared_ptr<gl::ShaderProgram> program) const {
    return {program, {{_vbo, _data.format}}};
  }

  // returns the generated EBO
  const gl::ElementBuffer& ebo() const {return _ebo;}

  // returns the given OpenGL draw mode
  gl::DrawMode draw_mode() const {return _draw_mode;}

private:
  MeshData _data;
  gl::DrawMode _draw_mode;
  gl::VertexBuffer _vbo;
  gl::ElementBuffer _ebo;
  float* _mapped_region;
};
