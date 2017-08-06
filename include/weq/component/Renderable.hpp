#pragma once

#include <weq/gl/ShaderProgram.hpp>
#include <weq/Mesh.hpp>

namespace component{

// Component describing all the data necessary for the render to draw a single
// mesh. Does currently not support materials.
struct Renderable{
  // Extracts relevant data from a given Mesh pointer. Renderable does not
  // take ownership of the allocated Mesh.
  Renderable(std::shared_ptr<Mesh> m)
    : mesh(m){
  }

  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<gl::ShaderProgram> scene;
};

} // namespace component
