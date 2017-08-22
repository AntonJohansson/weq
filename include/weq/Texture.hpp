#pragma once

#include <weq/resource/Resource.hpp>

#include <glad/glad.h>

#include <map>

// TODO CLEAN UP SPECIFICATION OF TEXTURE FORMAT
// Resource class for representing a texture that is useable
// by OpenGL.
// TODO make more customizable (able to change width etc.)
// TODO Should be able to support custom mip maps,
//      wrap mode, texture min/mag.
class Texture : public Resource{
public:
  // Contructs a texture resource with given id,
  // also generates the OpenGL texture id.
  Texture(const std::string& id, GLenum target);

  // Constructs an empty texture form memory.
  Texture(const std::string& id,
          GLenum target,
          unsigned int w,
          unsigned int h,
          GLenum internal = GL_RGB,
          GLenum external = GL_RGB,
          GLenum type = GL_UNSIGNED_BYTE,
          void* bits = nullptr);

  // Destorys the previously generated texture id.
  ~Texture();

  void set_format();

  // Set texture parameters as pairs of parameters and values.
  void set_parameters(std::map<GLenum, GLenum> params);

  // Handles loading the texture from file or memory to a
  // format useable by OpenGL.
  void load() override;

  // Unloads the texture data but keeps the texture id intact.
  void unload() override;

  // Binds and sets this texture as active on the specified index.
  void bind(int index = 0);

  // Returns the underlying OpenGL texture handle.
  GLuint handle(){return _texture;}

  // Resize the texture (only works if the texture is loaded from memory).
  void resize(unsigned int w, unsigned int h);

  // Returns the current width and height of the texture.
  unsigned int width() const {return _width;}
  unsigned int height() const {return _height;}

private:
  // Internal function that handles loading a texture with filename id from
  // disk. The returned tuple contains (raw data, width, height).
  std::tuple<unsigned char*, unsigned int, unsigned int>
  load_texture(const std::string& filename);

  GLenum _target;
  GLenum _format_internal;
  GLenum _format_external;
  GLenum _format_type;
  unsigned int _width;
  unsigned int _height;
  void* _bits;
  GLuint _texture;
};
