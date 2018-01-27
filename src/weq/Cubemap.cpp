#include <weq/Cubemap.hpp>

#include <spdlog/spdlog.h>
#include <FreeImage.h>

namespace weq{

Cubemap::Cubemap(const fs::path& path,
                 const std::vector<fs::path>& texture_ids)
  : Resource(path),
    _texture_ids(texture_ids){
  glGenTextures(1, &_cubemap);
}

Cubemap::~Cubemap(){
  unload();
}

void Cubemap::load(){
//  bind();
//
//  for(GLuint i = 0; i < _texture_ids.size(); i++){
//    auto [data, w, h] = load_texture(_resource_path + _texture_ids[i]);
//
//    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//                 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//  }
//
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::unload(){
}

void Cubemap::bind() const {
  glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemap);
}

std::tuple<unsigned char*, unsigned int, unsigned int>
Cubemap::load_texture(const std::string& id){
  FREE_IMAGE_FORMAT fif;
  FIBITMAP* data(0);
  BYTE* bits(0);

  fif = FIF_UNKNOWN;
  fif = FreeImage_GetFileType(id.c_str(), 0);

  if(fif == FIF_UNKNOWN){
    fif = FreeImage_GetFIFFromFilename(id.c_str());
  }

  if(fif == FIF_UNKNOWN){
    spdlog::get("console")->error("Unknown texture file format for: {}!", id);
    return {nullptr, 0, 0};
  }

  if(FreeImage_FIFSupportsReading(fif)){
    data = FreeImage_Load(fif, id.c_str());

    if(!data){
      spdlog::get("console")->error("Failed to read data for {}!", id);
      return {nullptr, 0, 0};
    }
  }else{
    spdlog::get("console")->error("FreeImage does not support this extension: {}!", id);
    return {nullptr, 0, 0};
  }

  bits = FreeImage_GetBits(data);
  unsigned int w = FreeImage_GetWidth(data);
  unsigned int h = FreeImage_GetHeight(data);

  FreeImage_Unload(data);

  return {bits, w, h};
}

} // namespace weq
