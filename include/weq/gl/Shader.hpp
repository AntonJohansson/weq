#pragma once

#include <weq/resource/Resource.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace gl{

enum ShaderType : GLuint{
  VERTEX   = GL_VERTEX_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER,
  GEOMETRY = GL_GEOMETRY_SHADER
};

class Shader : public Resource{
public:
  // Load from file with id
  Shader(const std::string& id, const std::string& file);
  // Relate id to in mem. source
  Shader(const std::string& id, ShaderType type, const std::string& source);
  ~Shader();

  void load() override;
  void unload() override;
  void compile();

  unsigned int id(){return _shader;}
  const ShaderType& type(){return _type;}

private:
  ShaderType type_from_filename(const std::string& filename) const;
  void delete_shader();
  std::string read_from_file(const std::string& file);

  std::string _filename;
  std::string _shader_source;
  ShaderType _type;
  unsigned int _shader;
};

};
