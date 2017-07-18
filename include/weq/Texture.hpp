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
  unsigned int _width;
  unsigned int _height;
  unsigned int _texture;
};
