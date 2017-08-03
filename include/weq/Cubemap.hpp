#pragma once

#include <weq/resource/Resource.hpp>
#include <vector>
#include <string>

// TODO Merge Cubemap and Texture resources?

// Combines six textures to form a cubemap
class Cubemap : public Resource{
public:
  // Constructs a resource with a given id and generates the OpenGL cubemap texture.
  // Copies texture_ids to local copy _texture_ids.
  Cubemap(const std::string& id,
          const std::vector<std::string>& texture_ids);
  // Calls unload() to destory all loaded textures.
  // Also destroys _cubemap texture.
  ~Cubemap();

  // Loads textures from memory or file and combines them to form
  // a cubemap.
  void load() override;
  // Frees the resources previously loaded.
  void unload() override;

  // Binds texture for use with OpenGL functions
  void bind() const;

private:
  // Internal function that handles loading a texture with filename id from
  // disk. The returned tuple contains (raw data, width, height).
  std::tuple<unsigned char*, unsigned int, unsigned int>
  load_texture(const std::string& id);

  std::vector<std::string> _texture_ids;
  unsigned int _cubemap;
};
