#include <weq/Texture.hpp>

#include <spdlog/spdlog.h>
#include <FreeImage.h>

Texture::Texture(const std::string& id, GLenum target)
  : Resource(id, ResourceType::FILE),
    _target(target) {
  glGenTextures(1, &_texture);
}

Texture::Texture(const std::string& id,
                 GLenum target,
                 unsigned int w,
                 unsigned int h,
                 GLenum internal,
                 GLenum external,
                 GLenum type,
                 void* bits)
  : Resource(id, ResourceType::MEMORY),
    _target(target),
    _format_internal(internal),
    _format_external(external),
    _format_type(type),
    _width(w),
    _height(h),
    _bits(bits){
  glGenTextures(1, &_texture);
}

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

void Texture::load(){
  _is_loaded = true;

  if(_type == ResourceType::FILE){
    const auto& [bits, width, height] = load_texture(_resource_path + _id);
    _width = width;
    _height = height;
    _bits = (void*)bits;
  }

  set_data(_bits);

  // TODO unload freeimage data after this.
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

std::tuple<unsigned char*, unsigned int, unsigned int>
Texture::load_texture(const std::string& filename){
  FREE_IMAGE_FORMAT fif;
  FIBITMAP* data(0);
  BYTE* bits(0);

  fif = FIF_UNKNOWN;
  fif = FreeImage_GetFileType(filename.c_str(), 0);

  if(fif == FIF_UNKNOWN){
    fif = FreeImage_GetFIFFromFilename(filename.c_str());
  }

  if(fif == FIF_UNKNOWN){
    spdlog::get("console")->error("Unknown texture file format for: {}!", filename);
    return {nullptr, 0, 0};
  }

  if(FreeImage_FIFSupportsReading(fif)){
    data = FreeImage_Load(fif, filename.c_str());

    if(!data){
      spdlog::get("console")->error("Failed to read data for {}!", filename);
      return {nullptr, 0, 0};
    }
  }else{
    spdlog::get("console")->error("FreeImage does not support this extension: {}!", filename);
    return {nullptr, 0, 0};
  }

  bits = FreeImage_GetBits(data);
  unsigned int w = FreeImage_GetWidth(data);
  unsigned int h = FreeImage_GetHeight(data);

  //FreeImage_Unload(data); TODO this should be called after glTexImage2D - could cause problems otherwise

  return {bits, w, h};
}
