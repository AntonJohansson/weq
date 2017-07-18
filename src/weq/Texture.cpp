#include <weq/Texture.hpp>

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <FreeImage.h>

Texture::Texture(const std::string& id)
  : Resource(id){
  glGenTextures(1, &_texture);
  FreeImage_Initialise(); //@TODO MOVE
}

Texture::~Texture(){
  glDeleteTextures(1, &_texture);
  FreeImage_DeInitialise(); //@TODO MOVE
}

void Texture::load(){
  auto path = _resource_path + _id;

  FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
  FIBITMAP* data(0);
  BYTE* bits(0);

  fif = FreeImage_GetFileType(path.c_str(), 0);

  if(fif == FIF_UNKNOWN){
    fif = FreeImage_GetFIFFromFilename(path.c_str());
  }

  if(fif == FIF_UNKNOWN){
    spdlog::get("console")->error("Unknown texture file format for: {}", path);
    return;
  }

  if(FreeImage_FIFSupportsReading(fif)){
    data = FreeImage_Load(fif, path.c_str());
  }

  if(!data){
    spdlog::get("console")->error("FreeImage does not support this extension: {}", path);
    return;
  }

  bits = FreeImage_GetBits(data);
  _width = FreeImage_GetWidth(data);
  _height= FreeImage_GetHeight(data);

  bind();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);

  FreeImage_Unload(data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::unload(){
}

void Texture::bind(){
  glBindTexture(GL_TEXTURE_2D, _texture);
}
