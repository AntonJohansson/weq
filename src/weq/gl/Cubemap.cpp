#include <weq/gl/Cubemap.hpp>
#include <weq/utility/NumberTypes.hpp>

//#include <stb/stb_image.h>

//#include <spdlog/spdlog.h>

namespace weq::gl{
Cubemap::Cubemap(const fs::path& id,
                 std::array<fs::path, 6> paths)
  : Resource(id),
    _paths(paths){
  glGenTextures(1, &_cubemap);
}

Cubemap::~Cubemap(){
  unload();
  glDeleteTextures(1, &_cubemap);
}

void Cubemap::load(){
  bind();

  // @TODO camera dir chanbed +z = up
  // +x right
  // -x left
  // +y back
  // -y front
  // +z top
  // -z bottom
  for(GLuint i = 0; i < _paths.size(); i++){
    i32 w, h, bpp;
    u8* data = stbi_load(_paths[i].string().c_str(), &w, &h, &bpp, 3);

    if(!data){
      spdlog::get("console")->info("Cubemap {} failed to load texture {}", _path.string(), _paths[i].string());
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::unload(){
}

void Cubemap::bind(int index) const {
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemap);
}

} // namespace weq
