#include <weq/gl/Framebuffer.hpp>
#include <weq/gl/Texture.hpp>

//#include <spdlog/spdlog.h>

namespace weq::gl{

Framebuffer::Framebuffer(unsigned int w, unsigned int h, GLenum internal, GLenum external, GLenum type){
     // Generate framebuffer
    glGenFramebuffers(1, &_id);
    bind();

    _texture = std::make_shared<Texture>(GL_TEXTURE_2D, w, h, internal, external, type);
    _texture->load();
    _texture->set_parameters({
        {GL_TEXTURE_BASE_LEVEL, 0},
        {GL_TEXTURE_MAX_LEVEL, 0},

        {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
        {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},

        {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
        });


    //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture->handle(), 0);

    // Necessary when using more than one color attachment.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    // Renderbuffer Depth
    // @TODO support resizing rbos
    glGenRenderbuffers(1, &_rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo_depth);

    if(!check_complete()){
      spdlog::get("console")->error("Failed to create a complete frambuffer {}!", _id);
    }

    unbind();
}

Framebuffer::~Framebuffer(){
  glDeleteFramebuffers(1, &_id);
  glDeleteRenderbuffers(1, &_rbo_depth);
}

bool Framebuffer::check_complete(){
  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

float Framebuffer::depth(float x, float y){
  float pixel;
  glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixel);

  return pixel;
}

void Framebuffer::resize(unsigned int w, unsigned int h){
  _texture->resize(w, h);

  // Resize depth buffer
  bind();
  glBindRenderbuffer(GL_RENDERBUFFER, _rbo_depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo_depth);

  // Check complete
  if(!check_complete()){
    spdlog::get("console")->error("Failed to create a complete frambuffer {}!", _id);
  }
  unbind();
}

void Framebuffer::bind(){
  glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void Framebuffer::unbind(){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace weq::gl
