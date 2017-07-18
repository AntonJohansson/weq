#pragma once

#include <weq/resource/Resource.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace gl{

enum ShaderType{
  VERTEX = GL_VERTEX_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER,
  GEOMETRY = GL_GEOMETRY_SHADER
};

class Shader : public Resource{
public:
  Shader(const std::string& id);
  ~Shader();

  void load() override;
  void unload() override;
  void compile();

  unsigned int id(){return _shader;}
  const ShaderType& type(){return _type;}

private:
  void delete_shader();
  std::string read_from_file(const std::string& file);

  ShaderType _type;
  unsigned int _shader;
};

};
