#pragma once

#include <weq/memory/Resource.hpp>

#include <glad/glad.h>

#include <string>
#include <filesystem>

namespace weq::gl{

// Forward declare of shader program, needed for hot-reloading
class ShaderProgram;

// Enum encapsulating the OpenGL shader types.
enum class ShaderType : GLenum{
  VERTEX   = GL_VERTEX_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER,
  GEOMETRY = GL_GEOMETRY_SHADER
};

// Resource class handling the loading (from memory or file) and compiling
// of an OpenGL shader.
class Shader : public memory::Resource{
public:
  // Creates a shader resource with id which should be loaded from file.
  // Automatically assumes shader type from file extension by calling
  // type_from_filename(...)
  Shader(const fs::path& path);

  // Creates a shader resource with id, ShaderType type and loads them
  // actual shader directly from memory.
  Shader(ShaderType type, const std::string& source);

  // Calls unload().
  ~Shader();

  // Loads the shader either from source or memory and calls compile().
  // Creates OpenGL shader handle.
  void load() override;

  // Unloads the shader from memory and deletes shader handle.
  void unload() override;

  // Compiles the shader source code. Assumes glShaderSource(...) has been
  // called, i.e. shader has been load()-ed.
  bool compile();

  // Returns the OpenGL shader handle.
  GLuint handle() const {return _shader;}

  // Returns the current shader type.
  const ShaderType& type(){return _shader_type;}

  // Currently keeping a pointer to the shader program in order to re-link program on reload
  void set_shader_program(ShaderProgram* _sp){_shader_program = _sp;}
  bool has_shader_program(){return _shader_program != nullptr;}
  ShaderProgram* shader_program(){return _shader_program;}

private:
  // Helper function to retrive the shader type from file extension.
  ShaderType type_from_filename(const fs::path& path) const;

  // Handles loading the shader source from file.
  std::string read_from_file(const fs::path& path);

  std::string _shader_source;
  ShaderType _shader_type;
  GLuint _shader;

  // Currently keeping a pointer to the shader program in order to re-link program on reload
  ShaderProgram* _shader_program;
};

} // namespace weq::gl
