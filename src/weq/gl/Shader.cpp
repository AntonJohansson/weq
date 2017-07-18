#include <weq/gl/Shader.hpp>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

namespace gl{

Shader::Shader(const std::string& id)
  : Resource(id){
  if(auto p = _id.find_last_of(".") ; p != std::string::npos){
    auto str = _id.substr(p);
    if     (str == ".vert"){_type = ShaderType::VERTEX;}
    else if(str == ".frag"){_type = ShaderType::FRAGMENT;}
    else if(str == ".geom"){_type = ShaderType::GEOMETRY;}
    else{spdlog::get("console")->error("Could not determine shader type for {} with extension {}!", _id, str);}
  }
}

Shader::~Shader(){
  unload();
}

void Shader::load(){
  if(_is_loaded){
  }

  auto source = read_from_file(_resource_path + _id);

  _shader = glCreateShader(_type);
  const auto* source_data = source.c_str(); // glShaderSource needs a pointer to a pointer for some reason
  glShaderSource(_shader, 1, &source_data, NULL);

  compile();
}

void Shader::unload(){
  delete_shader();
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


void Shader::delete_shader(){
  glDeleteShader(_shader);
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
