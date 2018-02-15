#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/event/Hotloader.hpp>
//#include <weq/Hotloader.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

namespace weq::gl{

Shader::Shader(const fs::path& path)
  : Resource(path){
  _shader_type = type_from_filename(path);
  _shader = glCreateShader(GLenum(_shader_type));

  // Support hotloading
//  weq::hotloader::add(_resource_path + std::string("shaders\\") + _id, [this](auto){
//      reload();
//
//      // relink program if this is a reload.
//      if(_is_loaded && _shader_program != nullptr){
//        _shader_program->link();
//      }
//    });
}

Shader::Shader(ShaderType type, const std::string& source)
  : _shader_source(source),
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

  if(_type == memory::ResourceType::FILE){
    // Load from file
    _shader_source = read_from_file(_path);
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

    spdlog::get("console")->error("Failed to compile shader {}!\nCompile logs:\n{}", _path.string(), buffer);
  }

  return status;
}

/*
 * Private functions
 */

ShaderType Shader::type_from_filename(const fs::path& path) const {
  auto ext = path.extension();

  if(ext == ".vert")return ShaderType::VERTEX;
  else if(ext == ".frag")return ShaderType::FRAGMENT;
  else if(ext == ".geom")return ShaderType::GEOMETRY;
  else{
    spdlog::get("console")->error("Could not determine shader type for {} with extension {}!", path.string(), ext.string());
  }

  // Assume vertex shader if extension is not found or supported, so that we don't error out,
  // maybe return a tuple with bool and shader.
  return ShaderType::VERTEX;
}

std::string Shader::read_from_file(const fs::path& path){
  std::ifstream input(path);

  if(input.is_open()){
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
  }else{
    spdlog::get("console")->error("Could not load file {}!", path.string());
    return "";
  }
}

} // namespace weq::gl
