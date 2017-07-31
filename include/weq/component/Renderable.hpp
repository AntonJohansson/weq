#pragma once

#include <weq/Mesh.hpp>

namespace component{

struct Renderable{
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

}
