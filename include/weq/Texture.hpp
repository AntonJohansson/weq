#pragma once

#include <weq/resource/Resource.hpp>

#include <glad/glad.h>

// Resource class for representing a texture that is useable
// by OpenGL.
class Texture : public Resource{
public:
  // Contructs a texture resource with given id,
  // also generates the OpenGL texture id.
  Texture(const std::string& id);

  // Destorys the previously generated texture id.
  ~Texture();

  // Handles loading the texture from file or memory to a
  // format useable by OpenGL.
  void load() override;

  // Unloads the texture data but keeps the texture id intact.
  void unload() override;

  void bind();

  unsigned int width() const {return _width;}
  unsigned int height() const {return _height;}

private:
  unsigned int _width;
  unsigned int _height;
  GLuint _texture;
};
