#pragma once

#include <weq/resource/Resource.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace gl{

// Enum encapsulating the OpenGL shader types.
enum class ShaderType : GLenum{
  VERTEX   = GL_VERTEX_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER,
  GEOMETRY = GL_GEOMETRY_SHADER
};

// Resource class handling the loading (from memory or file) and compiling
// of an OpenGL shader.
class Shader : public Resource{
public:
  // Creates a shader resource with id which should be loaded from file.
  // Automatically assumes shader type from file extension by calling
  // type_from_filename(...)
  Shader(const std::string& id);

  // Creates a shader resource with id, ShaderType type and loads them
  // actual shader directly from memory.
  Shader(const std::string& id, ShaderType type, const std::string& source);

  // Calls unload().
  ~Shader();

  // Loads the shader either from source or memory and calls compile().
  // Creates OpenGL shader handle.
  void load() override;

  // Unloads the shader from memory and deletes shader handle.
  void unload() override;

  // Compiles the shader source code. Assumes glShaderSource(...) has been
  // called, i.e. shader has been load()-ed.
  void compile();

  // Returns the OpenGL shader handle.
  GLuint id() const {return _shader;}

  // Returns the current shader type.
  const ShaderType& type(){return _type;}

private:
  // Helper function to retrive the shader type from file extension.
  ShaderType type_from_filename(const std::string& filename) const;

  // Handles loading the shader source from file.
  std::string read_from_file(const std::string& file);

  std::string _shader_source;
  ShaderType _type;
  GLuint _shader;
};

} // namespace gl
