#pragma once

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace gl{

enum ShaderType{
  VERTEX = GL_VERTEX_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER
};

class Shader{
public:
  Shader();
  Shader(const std::string& v, const std::string& f);
  ~Shader();

  void load(const std::string& s, ShaderType type);
  void link();

  void use();

  void bind_attribute(const std::string& attribute,
                      unsigned int type,
                      unsigned int size,
                      unsigned int stride,
                      unsigned int offset);

  void set(const std::string& name, glm::mat4 mat);
  void set(const std::string& name, glm::vec3 vec);
  void set(const std::string& name, glm::vec4 vec);
  void set(const std::string& name, float f);

private:
  void delete_shader(GLuint shader);
  void delete_program();
  std::string read_from_file(const std::string& file);

  static constexpr auto _resource_path = "/Users/antonjohansson/git/WaveEquationFDM/res/";
  std::unordered_map<unsigned int, GLuint> _shaders;
  GLuint _program;
};

};
