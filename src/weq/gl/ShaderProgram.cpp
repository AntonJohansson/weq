#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace gl{

ShaderProgram::ShaderProgram(const std::string& id)
  : Resource(id){
}

ShaderProgram::ShaderProgram(const std::string& id,
                             std::shared_ptr<gl::Shader> v,
                             std::shared_ptr<gl::Shader> f,
                             std::shared_ptr<gl::Shader> g)
  : Resource(id){
  if(v)_shaders[v->type()] = v;
  if(f)_shaders[f->type()] = f;
  if(g)_shaders[g->type()] = g;
}

ShaderProgram::~ShaderProgram(){
  unload();
}

void ShaderProgram::link(){
  glLinkProgram(_program);

  // Link status
  GLint status;
  glGetProgramiv(_program, GL_LINK_STATUS, &status);

  if(status == GL_TRUE){
    use();
  }else{
    char buffer[512];
    glGetProgramInfoLog(_program, 512, NULL, buffer);

    spdlog::get("console")->error("Failed to link program {}!\nLink log:\n{}", _id, buffer);

    delete_program();
  }
}

void ShaderProgram::load(){
  _program = glCreateProgram();

  for(auto& shader : _shaders){
    glAttachShader(_program, shader.second->id());
  }

  glBindFragDataLocation(_program, 0, "frag_color");
}

void ShaderProgram::unload(){
  for(auto& shader : _shaders){
    glDetachShader(_program, shader.second->id());
  }

  delete_program();
}

void ShaderProgram::use(){
  glUseProgram(_program);
}

void ShaderProgram::bind_attribute(const std::string& attribute,
                                   unsigned int type,
                                   unsigned int size,
                                   unsigned int stride,
                                   unsigned int offset){
  auto attrib_location = glGetAttribLocation(_program, attribute.c_str());
  glVertexAttribPointer(attrib_location, size, type, GL_FALSE, stride, (void*)offset);
  glEnableVertexAttribArray(attrib_location);
}

void ShaderProgram::set_feedback(std::vector<const char*> varyings){
  glTransformFeedbackVaryings(_program, varyings.size(), varyings.data(), GL_INTERLEAVED_ATTRIBS);
}

void ShaderProgram::set(const std::string& name, glm::mat4 mat){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::set(const std::string& name, glm::vec3 vec){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform3fv(uniform, 1, glm::value_ptr(vec));
}

void ShaderProgram::set(const std::string& name, glm::vec4 vec){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform4fv(uniform, 1, glm::value_ptr(vec));
}

void ShaderProgram::set(const std::string& name, float f){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform1f(uniform, f);
}

void ShaderProgram::delete_program(){
  glDeleteProgram(_program);
}

}
