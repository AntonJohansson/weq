#pragma once

#include <weq/resource/Resource.hpp>

#include <glm/glm.hpp>

#include <unordered_map>
#include <memory>

namespace gl{

class Shader;

class ShaderProgram : public Resource{
public:
  ShaderProgram(const std::string& id);
  ShaderProgram(const std::string& id,
                std::shared_ptr<gl::Shader> v,
                std::shared_ptr<gl::Shader> f);
  ~ShaderProgram();

  void link();
  void load() override;
  void unload() override;

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
  void delete_program();

  unsigned int _program;
  std::unordered_map<unsigned int, std::shared_ptr<Shader>> _shaders;
};

}