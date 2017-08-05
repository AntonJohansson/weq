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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

    // Necessary when using more than one color attachment.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    // Renderbuffer Depth
    glGenRenderbuffers(1, &_rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    //glRenderBufferStorage(GL_RENDER_BUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo_depth);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo_depth);

    if(!check_complete()){
      spdlog::get("console")->error("Failed to create frambuffer {}!", _id);
    }

    unbind();
  }

  // Destroys the framebuffer, render buffers and texutres.
  ~Framebuffer(){
    glDeleteFramebuffers(1, &_id);
    glDeleteRenderbuffers(1, &_rbo_depth);
    glDeleteTextures(1, &_texture);
  }

  // Returns true if the framebuffer creation was successful.
  bool check_complete(){
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
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

  // Returns internal texture handle.
  GLuint texture(){return _texture;}

private:
  GLuint _rbo_depth;
  GLuint _texture;
  GLuint _id;
};

} // namespace gl
