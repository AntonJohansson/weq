#include <weq/gl/Shader.hpp>

//#include <OpenGL/gl3.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

namespace gl{

Shader::Shader(){
}

Shader::Shader(const std::string& v, const std::string& f){
  load(_resource_path + v, ShaderType::VERTEX);
  load(_resource_path + f, ShaderType::FRAGMENT);
  link();
}

Shader::~Shader(){
  for(auto shader : _shaders){
    delete_shader(shader.second);
  }

  delete_program();
}

void Shader::load(const std::string& s, ShaderType type){
  auto source = read_from_file(s);

  if(_shaders.find(type) != _shaders.end()){ // delete shader if it already exists
    delete_shader(_shaders[type]);
  }

  auto shader = glCreateShader(type);
  const auto* source_data = source.c_str(); // glShaderSource needs a pointer to a pointer for some reason
  glShaderSource(shader, 1, &source_data, NULL);
  glCompileShader(shader);

  // check compile status
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if(status == GL_TRUE){
    _shaders[type] = shader;
  }else{
    std::cerr << "Failed to compile shader: " << s << std::endl;

    char buffer[512];
    glGetShaderInfoLog(shader, 512, NULL, buffer);

    std::cerr << "----| Compile Logs |----\n";
    std::cerr << buffer;
    std::cerr << "------------------------\n";

    delete_shader(shader);
  }
}

void Shader::link(){
  delete_program();

  _program = glCreateProgram();

  for(auto& shader : _shaders){
    glAttachShader(_program, shader.second);
  }

  glBindFragDataLocation(_program, 0, "frag_color");
  glLinkProgram(_program);

  // Link status
  GLint status;
  glGetProgramiv(_program, GL_LINK_STATUS, &status);

  if(status == GL_TRUE){
    use();
  }else{
    std::cerr << "Failed to link program!" << std::endl;

    char buffer[512];
    glGetProgramInfoLog(_program, 512, NULL, buffer);

    std::cerr << "----| Link Logs |----\n";
    std::cerr << buffer;
    std::cerr << "----------------------\n";

    delete_program();
  }
}

void Shader::use(){
  glUseProgram(_program);
}

void Shader::bind_attribute(const std::string& attribute,
                            unsigned int type,
                            unsigned int size,
                            unsigned int stride,
                            unsigned int offset)
{
  auto attrib_location = glGetAttribLocation(_program, attribute.c_str());
  glVertexAttribPointer(attrib_location, size, type, GL_FALSE, stride, (void*)offset);
  glEnableVertexAttribArray(attrib_location);
}

void Shader::set(const std::string& name, glm::mat4 mat){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set(const std::string& name, glm::vec3 vec){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform3fv(uniform, 1, glm::value_ptr(vec));
}

void Shader::set(const std::string& name, glm::vec4 vec){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform4fv(uniform, 1, glm::value_ptr(vec));
}

void Shader::set(const std::string& name, float f){
  auto uniform = glGetUniformLocation(_program, name.c_str());
  glUniform1f(uniform, f);
}

void Shader::delete_shader(GLuint shader){
  if(glIsShader(shader)){
    glDetachShader(_program, shader);
    glDeleteShader(shader);
  }
}

void Shader::delete_program(){
  if(glIsProgram(_program)){
    glDeleteProgram(_program);
  }
}

std::string Shader::read_from_file(const std::string& file){
  std::ifstream input(file);

  if(input.is_open()){
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
  }else{
    std::cerr << "Shader file not found: " << file << std::endl;
    return "";
  }
}

};
