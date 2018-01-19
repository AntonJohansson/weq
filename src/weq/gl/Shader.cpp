#include <weq/gl/Shader.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <weq/Hotloader.hpp>
#include <weq/gl/ShaderProgram.hpp>

namespace gl{

Shader::Shader(const std::string& id)
  : Resource(id, ResourceType::FILE){
  _shader_type = type_from_filename(_id);
  _shader = glCreateShader(GLenum(_shader_type));

  // Support hotloading
  weq::hotloader::add(_resource_path + std::string("shaders\\") + _id, [this](auto){
      reload();

      // relink program if this is a reload.
      if(_is_loaded && _shader_program != nullptr){
        _shader_program->link();
      }
    });
}

Shader::Shader(const std::string& id, ShaderType type, const std::string& source)
  : Resource(id, ResourceType::MEMORY),
    _shader_source(source),
    _shader_type(type){
  _shader = glCreateShader(GLenum(_shader_type));
}

Shader::~Shader(){
  glDeleteShader(_shader);
}

void Shader::load(){
  if(_is_loaded){
    return;
  }

  if(_type == ResourceType::FILE){
    // Load from file
    _shader_source = read_from_file(_resource_path + std::string("shaders\\") + _id);
  }

  const GLchar* source = _shader_source.c_str();
  glShaderSource(_shader, 1, &source, NULL); // NULL = string length, NULL = > null terminated :)

  if(compile()){
    _is_loaded = true;
  }
}

void Shader::unload(){
  _is_loaded = false;
}

bool Shader::compile(){
  glCompileShader(_shader);

  // check compile status
  GLint status;
  glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);

  if(!status){
    GLchar buffer[512];
    glGetShaderInfoLog(_shader, 512, NULL, buffer);

    spdlog::get("console")->error("Failed to compile shader {}!\nCompile logs:\n{}", _id, buffer);
  }

  return status;
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

} // namespace gl
