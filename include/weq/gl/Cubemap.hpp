#pragma once

#include <weq/memory/Resource.hpp>

#include <glad/glad.h>

#include <array>

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


namespace weq::gl{


// TODO Merge Cubemap and Texture resources?

// Combines six textures to form a cubemap
class Cubemap : public memory::Resource{
public:
  // Constructs a resource with a given id and generates the OpenGL cubemap texture.
  // Copies texture_ids to local copy _texture_ids.
  Cubemap(const fs::path& id,
          std::array<fs::path, 6> paths);
  // Calls unload() to destory all loaded textures.
  // Also destroys _cubemap texture.
  ~Cubemap();

  // Loads textures from memory or file and combines them to form
  // a cubemap.
  void load() override;
  // Frees the resources previously loaded.
  void unload() override;

  // Binds texture for use with OpenGL functions
  void bind(int index = 0) const;

private:
  std::array<fs::path, 6> _paths;
  GLuint _cubemap;
};

} // namespace weq
