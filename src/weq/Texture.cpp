#include <weq/Texture.hpp>

#include <glad/glad.h>
#include <SOIL/SOIL.h>
#include <spdlog/spdlog.h>

Texture::Texture(const std::string& id)
  : Resource(id){
  glGenTextures(1, &_texture);
}

Texture::~Texture(){
  glDeleteTextures(1, &_texture);
}

void Texture::load(){
  auto path = _resource_path + _id;
  _data = SOIL_load_image(path.c_str(), &_width, &_height, 0, SOIL_LOAD_RGB);

  if(_data == nullptr){
    spdlog::get("console")->error("Texture file not found {}!\nError: {}", path, SOIL_last_result());
    return;
  }

  bind();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _data);

  SOIL_free_image_data(_data);

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
