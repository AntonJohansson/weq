#pragma once

#include <weq/resource/Resource.hpp>

class Texture : public Resource{
public:
  Texture(const std::string& id);
  ~Texture();

  void load() override;
  void unload() override;

  void bind();

private:
  int _width;
  int _height;
  unsigned char* _data;
  unsigned int _texture;
};
