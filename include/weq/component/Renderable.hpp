#pragma once

#include <weq/Mesh.hpp>

namespace component{

struct Renderable{
  Renderable(Mesh* m, gl::Shader* s)
    : mesh(m),
      shader(s){}
  Mesh* mesh;
  gl::Shader* shader;
};

}
