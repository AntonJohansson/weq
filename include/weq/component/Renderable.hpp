#pragma once

#include <weq/gl/ShaderProgram.hpp>
#include <weq/Mesh.hpp>
#include <weq/gl/Texture.hpp>

#include <vector>

namespace weq::component{

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
  // TODO there is a better data type for this.
  // should also keep track of name to bind to in shaders.
  std::vector<std::shared_ptr<gl::Texture>> textures;
  // @TODO this is probably not the best solution.
  bool require_skybox     = false;
  bool require_camera_pos = false;
  bool wireframe          = false;

};

} // namespace weq::component
