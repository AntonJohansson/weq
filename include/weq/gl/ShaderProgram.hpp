#pragma once

#include <weq/memory/Resource.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <memory>
#include <vector>
#include <filesystem>

namespace weq::gl{

namespace fs = std::experimental::filesystem;

// Defined in <weq/gl/Shader.hpp>
class Shader;

// Resource that links already compiled shaders into a functioning shader program.
// TODO might be cool to give a way to extract vertex-attribs and shader output
//      from source?
// TODO better error checking (loaded or not).
class ShaderProgram : public memory::Resource{
public:
  // Constructs a shaderprogram with the given id and specified shaders.
  // Creates the OpenGL handle for the shader program.
  ShaderProgram(std::shared_ptr<gl::Shader> v = nullptr,
                std::shared_ptr<gl::Shader> f = nullptr,
                std::shared_ptr<gl::Shader> g = nullptr);

  // Calls unload() to detach shaders, also destorys the OpenGL handle.
  ~ShaderProgram();

  // Links the attached shaders and binds the fragdata location to
  // "frag_color."
  void link();

  // Attaches shaders to program and loads unloaded ones, unless
  // previous loading failed.
  void load() override;

  // Detaches all shaders from the program.
  void unload() override;

  void use();

  // Binds (type, ..., offset) to the vertex attribute corresponding to
  // the attribute string.
  //   bind_attribute("position", GL_FLOAT, 3*sizeof(float), 0, 0)
  //     ->
  //   in vec3 position.
  void bind_attribute(const std::string& attribute,
                      GLuint type,
                      GLuint size,
                      GLuint stride,
                      GLuint offset);

  // Sets the feedback varyings to be used during Transform Feedback.
  // Needs to be called before the program is linked.
  void set_feedback(std::vector<const char*> varyings);

  // Set uniforms
  void set(const std::string& name, glm::mat4 mat);
  void set(const std::string& name, glm::vec2 vec);
  void set(const std::string& name, glm::vec3 vec);
  void set(const std::string& name, glm::vec4 vec);
  void set(const std::string& name, double d);
  void set(const std::string& name, float f);
  void set(const std::string& name, int i);

  // Get program handle
  GLuint handle(){return _program;}

private:
  GLuint _program;
  std::unordered_map<GLenum, std::shared_ptr<Shader>> _shaders;
};

} // namespace weq::gl
