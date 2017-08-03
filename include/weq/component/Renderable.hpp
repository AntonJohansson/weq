#pragma once

#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/DrawModes.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/VertexArray.hpp>
#include <weq/Mesh.hpp>

namespace component{

// Component describing all the data necessary for the render to draw a single
// mesh. Does currently not support materials.
struct Renderable{
  // Extracts relevant data from a given Mesh pointer. Renderable does not
  // take ownership of the allocated Mesh.
  Renderable(Mesh* m, std::shared_ptr<gl::ShaderProgram> p)
    : program(p){
    draw_mode = m->draw_mode();
    ebo = m->ebo();
    vao = m->vao(program);
  }

  std::shared_ptr<gl::ShaderProgram> program;
  gl::DrawMode draw_mode;
  gl::ElementBuffer ebo;
  gl::VertexArray vao;
};

} // namespace component
