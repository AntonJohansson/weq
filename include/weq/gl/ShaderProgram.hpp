#pragma once

#include <weq/resource/Resource.hpp>

#include <glm/glm.hpp>

#include <unordered_map>
#include <memory>
#include <vector>

namespace gl{

class Shader;

class ShaderProgram : public Resource{
public:
  ShaderProgram(const std::string& id);
  ShaderProgram(const std::string& id,
                std::shared_ptr<gl::Shader> v = nullptr,
                std::shared_ptr<gl::Shader> f = nullptr,
                std::shared_ptr<gl::Shader> g = nullptr);
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

  void set_feedback(std::vector<const char*> varyings);
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
