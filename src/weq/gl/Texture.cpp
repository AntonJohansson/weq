#include <weq/gl/Texture.hpp>
#include <weq/utility/NumberTypes.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION // to support unity-build

#include <spdlog/spdlog.h>

namespace weq::gl{

Texture::Texture(const fs::path& path)
   : Resource(path),
    _target(GL_TEXTURE_2D),
    _format_internal(GL_RGB),
    _format_external(GL_RGB),
    _format_type(GL_UNSIGNED_BYTE){
  glGenTextures(1, &_texture);
}

Texture::Texture(GLenum target,
                 unsigned int w,
                 unsigned int h,
                 GLenum internal,
                 GLenum external,
                 GLenum type,
                 void* bits)
  : _target(target),
    _format_internal(internal),
    _format_external(external),
    _format_type(type),
    _width(w),
    _height(h),
    _bits(bits){
  glGenTextures(1, &_texture);
}

Texture::Texture(const fs::path& path, GLuint texture, GLenum target)
  : Resource(path),
    _texture(texture),
    _target(target){}

Texture::~Texture(){
  glDeleteTextures(1, &_texture);
}

void Texture::set_parameters(std::map<GLenum, GLenum> params){
  bind();
  for(auto& pair : params){
    glTexParameteri(_target, pair.first, pair.second);
  }
}

void Texture::set_data(void* bits){
  bind();

  // I think OpenGL manages this memory.
  //if(_bits != nullptr){
  //  delete _bits;
  //}

  _bits = bits;

  //glTexSubImage2D(_target, 0, 0, 0, _width, _height, _format_external, _format_type, _bits);
  glTexImage2D(_target, 0, _format_internal, _width, _height, 0, _format_external, _format_type, _bits);
}

void Texture::set_subdata(int x, int y, int width, int height, void* bits){
  bind();
  // assert that 0 < x + w < width, 0 < y + h < height
  //if(x + width > _width)width = _width - x;
  //if(y + height > _height)height = _height - y;
  //spdlog::get("console")->info("{} + {} > {}",x, width, _

  glTexSubImage2D(_target, 0, x, y, width, height, _format_external, _format_type, bits);
}

void Texture::get_subdata(void* bits, int size, int x, int y, int width, int height){
  //if(x + width > _width)width = _width - x;
  //if(y + height > _height)height = _height - y;

  glGetTextureSubImage(_texture, 0, x, y, 0, width, height, 1, _format_external, _format_type, size, bits);
}

void Texture::load(){
  _is_loaded = true;

  if(_type == memory::ResourceType::FILE){
    load_texture(_path);
  }

  set_data(_bits);

  // Free data, it has been copied to OpenGL!
  if(_type == memory::ResourceType::FILE)stbi_image_free(_bits);
}

void Texture::unload(){
  _is_loaded = false;
}

void Texture::bind(int index){
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(_target, _texture);
}

void Texture::resize(unsigned int w, unsigned int h){
  if(_is_loaded){
    _width = w;
    _height = h;

    delete _bits;
    _bits = nullptr;

    reload();
  }
}

// Private functions
void Texture::load_texture(const fs::path& path){
  i32 w, h, bpp;
  _bits = stbi_load(path.string().c_str(), &w, &h, &bpp, 3);

  if(_bits){
    _width  = static_cast<u32>(w);
    _height = static_cast<u32>(h);
  }else{
    spdlog::get("console")->info("Failed to load data for {}", path.string());
  }
}

} // namespace weq::gl
