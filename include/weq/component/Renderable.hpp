#pragma once

#include <weq/Mesh.hpp>

namespace component{

struct Renderable{
  Renderable(Mesh* m, std::shared_ptr<gl::ShaderProgram> p)
    : mesh(m),
      program(p){}
  Mesh* mesh;
  std::shared_ptr<gl::ShaderProgram> program;
};

}
