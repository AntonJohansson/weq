#include <weq/Texture.hpp>

#include <spdlog/spdlog.h>
#include <FreeImage.h>

Texture::Texture(const std::string& id)
  : Resource(id, ResourceType::FILE){
  glGenTextures(1, &_texture);
}

Texture::Texture(const std::string& id,
                 unsigned int w,
                 unsigned int h,
                 unsigned char* bits)
  : Resource(id, ResourceType::MEMORY),
    _width(w),
    _height(h),
    _bits(bits){
  glGenTextures(1, &_texture);
}

Texture::~Texture(){
  glDeleteTextures(1, &_texture);
}

void Texture::load(){
  _is_loaded = true;

  if(_type == ResourceType::FILE){
    const auto& [bits, width, height] = load_texture(_resource_path + _id);
    _width = width;
    _height = height;
    _bits = bits;
  }

  bind();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _bits);

  // Mipmaps
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  // Wrap mode
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // Filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::unload(){
  _is_loaded = false;
}

void Texture::bind(int index){
  switch(index){
  case 0: glActiveTexture(GL_TEXTURE0); break;
  case 1: glActiveTexture(GL_TEXTURE1); break;
  case 2: glActiveTexture(GL_TEXTURE2); break;
  case 3: glActiveTexture(GL_TEXTURE3); break;
  };

  glBindTexture(GL_TEXTURE_2D, _texture);
}

void Texture::resize(unsigned int w, unsigned int h){
  if(_is_loaded){
    _width = w;
    _height = h;
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

  FreeImage_Unload(data);

  return {bits, w, h};
}
