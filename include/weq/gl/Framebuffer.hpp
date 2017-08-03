#pragma once

#include <glad/glad.h>
#include <spdlog/spdlog.h>

namespace gl{

// Abstraction of a common OpenGL framebuffer.
// Also supports depth and stencil render targets
// (not yet tho lol).
// TODO integrate with existing texture class.
class Framebuffer{
public:
  // Creates a framebuffer with the specified width and height.
  // Handles creating the actual framebuffer, textures and
  // render targets.
  // TODO split.
  Framebuffer(unsigned int w, unsigned int h){
    // Generate framebuffer
    glGenFramebuffers(1, &_id);
    bind();

    // Setup texture
    glGenTextures(1, &_texture);
    bind_texture();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /* Depth buffer
       unsigned int depth_render_buffer;
       glGenRenderBuffers(1, &depth_render_buffer);
       glBindRenderBuffer(GL_RENDERBUFFER, depth_render_buffer);
       glRenderBufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
       glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer);
       ...
       glDeleteRenderbuffers(depth_render_buffer);
    */

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texture, 0);

    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);

    if(check_complete()){
      spdlog::get("console")->error("Failed to create frambuffer {}!", _id);
    }
  }

  // Destroys the framebuffer, render buffers and texutres.
  ~Framebuffer(){
    glDeleteFramebuffers(1, &_id);
    glDeleteTextures(1, &_texture);
  }

  // Returns true if the framebuffer creation was successful.
  bool check_complete(){
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE;
  }

  // Binds the current framebuffer handle for use with OpenGL functions,
  // assumes the framebuffer was created correctly.
  void bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
  }

  // Binds the related render texture.
  // Assumes that the texture was created correctly.
  void bind_texture(){
    glBindTexture(GL_TEXTURE_2D, _texture);
  }

  // Unbinds the current framebuffer. Effectively binding
  // the default framebuffer instead.
  void unbind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

private:
  GLuint _texture;
  GLuint _id;
};

} // namespace gl
