#pragma once

#include <glad/glad.h>
#include <memory>

class Texture;

namespace gl{

// Abstraction of a common OpenGL framebuffer.
// Also supports depth and stencil render targets
// (not yet tho lol).
// TODO Make more customizable.
// TODO support multisampling.
// TODO support resizing.
class Framebuffer{
public:
  // Creates a framebuffer with the specified width and height.
  // Handles creating the actual framebuffer, textures and
  // render targets.
  // TODO split.
  Framebuffer(unsigned int w, unsigned int h);

  // Destroys the framebuffer, render buffers and texutres.
  ~Framebuffer();

  // Returns true if the framebuffer creation was successful.
  bool check_complete();

  // Returns a pointer to the underlying fbo texture.
  std::shared_ptr<Texture> texture(){return _texture;}

  // Binds the current framebuffer.
  void bind();

  // Unbinds the current framebuffer. Effectively binding
  // the default framebuffer instead.
  void unbind();

private:
  std::shared_ptr<Texture> _texture;
  GLuint _rbo_depth;
  GLuint _id;
};

} // namespace gl
