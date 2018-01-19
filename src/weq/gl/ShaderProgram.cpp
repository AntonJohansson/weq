#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace gl{

ShaderProgram::ShaderProgram(const std::string& id)
  : Resource(id, ResourceType::FILE){
  _program = glCreateProgram();
}

ShaderProgram::ShaderProgram(const std::string& id,
                             std::shared_ptr<gl::Shader> v,
                             std::shared_ptr<gl::Shader> f,
                             std::shared_ptr<gl::Shader> g)
  : Resource(id, ResourceType::MEMORY){
  if(v)_shaders[GLenum(v->type())] = v;
  if(f)_shaders[GLenum(f->type())] = f;
  if(g)_shaders[GLenum(g->type())] = g;

  _program = glCreateProgram();
}

ShaderProgram::~ShaderProgram(){
  unload();

  if(glIsProgram(_program)){
    glDeleteProgram(_program);
  }
}

void ShaderProgram::link(){
  glLinkProgram(_program);

  // Link status
  GLint status;
  glGetProgramiv(_program, GL_LINK_STATUS, &status);

  if(status){
    use();
  }else{
    GLchar buffer[512];
    glGetProgramInfoLog(_program, 512, NULL, buffer);

    spdlog::get("console")->error("Failed to link program {}!\nLink log:\n{}", _id, buffer);
  }
}

void ShaderProgram::load(){
  for(auto& shader : _shaders){
    glAttachShader(_program, shader.second->id());
    // TEMPORARY
    shader.second->set_shader_program(this);
  }

  glBindFragDataLocation(_program, 0, "frag_color");
}

void ShaderProgram::unload(){
  for(auto& shader : _shaders){
    glDetachShader(_program, shader.second->id());
  }
}

void ShaderProgram::use(){
  glUseProgram(_program);
}

void ShaderProgram::bind_attribute(const std::string& attribute,
                                   GLuint type,
                                   GLuint size,
                                   GLuint stride,
                                   GLuint offset){
  GLvoid const* offset_pointer = static_cast<char const*>(0) + offset;

  auto attrib_location = glGetAttribLocation(_program, attribute.c_str());

  // Error check
  if(attrib_location >= 0){ // TODO && < MAX
    glVertexAttribPointer(attrib_location, size, type, GL_FALSE, stride, offset_pointer);
    glEnableVertexAttribArray(attrib_location);
  }else{
    spdlog::get("console")->error("Shader Error: Attribute \"{}\" cannot be found for shader {}.\n\t Either it is being optimzed out, or isn't being declared.", attribute, _id);
  }
}

void ShaderProgram::set_feedback(std::vector<const GLchar*> varyings){
  glTransformFeedbackVaryings(_program, varyings.size(), varyings.data(), GL_INTERLEAVED_ATTRIBS);
}

void ShaderProgram::set(const std::string& name, glm::mat4 mat){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::set(const std::string& name, glm::vec2 vec){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform2fv(uniform, 1, glm::value_ptr(vec));
}

void ShaderProgram::set(const std::string& name, glm::vec3 vec){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform3fv(uniform, 1, glm::value_ptr(vec));
}

void ShaderProgram::set(const std::string& name, glm::vec4 vec){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform4fv(uniform, 1, glm::value_ptr(vec));
}

void ShaderProgram::set(const std::string& name, double d){
  set(name, static_cast<float>(d));
}

void ShaderProgram::set(const std::string& name, float f){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform1f(uniform, f);
}

void ShaderProgram::set(const std::string& name, int i){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform1i(uniform, i);
}

} // namespace gl
