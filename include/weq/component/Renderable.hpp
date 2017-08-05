#pragma once

#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/DrawModes.hpp>
#include <weq/gl/Buffer.hpp>
#include <weq/gl/VertexArray.hpp>
#include <weq/gl/Framebuffer.hpp>
#include <weq/Mesh.hpp>

namespace component{

// Component describing all the data necessary for the render to draw a single
// mesh. Does currently not support materials.
struct Renderable{
  // Extracts relevant data from a given Mesh pointer. Renderable does not
  // take ownership of the allocated Mesh.
  Renderable(std::shared_ptr<Mesh> m)
    : mesh(m){
    draw_mode = m->draw_mode();
  }

  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Mesh> screen_mesh;
  //std::shared_ptr<gl::ShaderProgram> program;
  std::shared_ptr<gl::ShaderProgram> screen;
  std::shared_ptr<gl::ShaderProgram> scene;
  gl::DrawMode draw_mode;
  gl::Framebuffer fbo{1280, 720};
};

} // namespace component
