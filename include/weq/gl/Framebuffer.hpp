#pragma once

#include <glad/glad.h>
#include <memory>



namespace weq::gl{

class Texture;

// Abstraction of a common OpenGL framebuffer.
// Also supports depth and stencil render targets
// (not yet tho lol).
// TODO Make more customizable.
// TODO support multisampling.
// TODO support resizing.
// TODO all FBOs have depth components by default, might not be desirable. :)
class Framebuffer{
public:
  // Creates a framebuffer with the specified width and height.
  // Handles creating the actual framebuffer, textures and
  // render targets.
  // TODO split.
  Framebuffer(unsigned int w, unsigned int h, GLenum internal = GL_RGB,
              GLenum external = GL_RGB, GLenum type = GL_UNSIGNED_BYTE);

  // Destroys the framebuffer, render buffers and texutres.
  ~Framebuffer();

  // Returns true if the framebuffer creation was successful.
  bool check_complete();

  // Reads the depth buffer (if the fbo has one) at
  // coordnates (x, y).
  // TODO This should obviosly not work if the fbo doesn't support depth.
  float depth(float x, float y);

  // Resizes frambuffer attachments
  void resize(unsigned int w, unsigned int h);

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

} // namespace weq::gl
