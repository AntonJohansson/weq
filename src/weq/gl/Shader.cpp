#include <weq/gl/Shader.hpp>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace gl{

Shader::Shader(const std::string& id, const std::string& filename)
  : Resource(id),
    _filename(filename){
  _type = type_from_filename(_filename);
}

Shader::Shader(const std::string& id, ShaderType type, const std::string& source)
  : Resource(id),
    _shader_source(source),
    _type(type){
  _shader = glCreateShader(_type);
}

Shader::~Shader(){
  unload();
}

void Shader::load(){
  if(_is_loaded){
  }

  if(_shader_source.empty()){
    // Load from file
    _shader_source = read_from_file(_resource_path + _filename);
  }

  _shader = glCreateShader(_type);
  const char* source = _shader_source.c_str();
  glShaderSource(_shader, 1, &source, NULL);

  compile();
}

void Shader::unload(){
  glDeleteShader(_shader);
}

void Shader::compile(){
  glCompileShader(_shader);

  // check compile status
  GLint status;
  glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);

  if(status == GL_TRUE){
  }else{
    char buffer[512];
    glGetShaderInfoLog(_shader, 512, NULL, buffer);

    spdlog::get("console")->error("Failed to compile shader {}!\nCompile logs:\n{}", _id, buffer);

    delete_shader();
  }
}

/*
 * Private functions
 */

ShaderType Shader::type_from_filename(const std::string& filename) const {
  if(auto p = filename.find_last_of(".") ; p != std::string::npos){
    auto str = filename.substr(p);
    if     (str == ".vert"){return ShaderType::VERTEX;}
    else if(str == ".frag"){return ShaderType::FRAGMENT;}
    else if(str == ".geom"){return ShaderType::GEOMETRY;}
    else{
      spdlog::get("console")->error("Could not determine shader type for {} with extension {}!", _id, str);
    }
  }

  // TODO How should I handle this? Only expections? Optionals?
  throw std::runtime_error("Could not determine shader type!");
}

std::string Shader::read_from_file(const std::string& file){
  std::ifstream input(file);

  if(input.is_open()){
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
  }else{
    spdlog::get("console")->error("Could not load file {}!", file);
    return "";
  }
}

};
