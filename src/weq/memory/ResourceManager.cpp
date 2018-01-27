#include <weq/memory/ResourceManager.hpp>
#include <weq/memory/Resource.hpp>

#include <weq/gl/Texture.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>

#include <weq/event/Hotloader.hpp>

#include <spdlog/spdlog.h>
#include <FreeImage.h>

#include <filesystem>
#include <unordered_map>

namespace weq::memory::resource_manager{

namespace{
  const fs::path _base_path = "../res";
  const fs::path _shader_path = _base_path/fs::path("/shaders");

  ex::EventManager* _events;
}

std::unordered_map<std::string, std::weak_ptr<Resource>> _memory;

void initialize(ex::EventManager& events){
  _events = &events;

  FreeImage_Initialise();
}

void shutdown(){
  FreeImage_DeInitialise();
}

std::shared_ptr<gl::ShaderProgram> load_shader_program(const fs::path& id){
  auto path = (_shader_path/id);
  auto vert_path = path.replace_extension(".vert");
  auto frag_path = path.replace_extension(".frag");
  auto vert = get<gl::Shader>(vert_path.filename().string(), vert_path);
  auto frag = get<gl::Shader>(frag_path.filename().string(), frag_path);

  auto ptr = get<gl::ShaderProgram>(id.string(), vert, frag);
  ptr->link();

  return ptr;
}

std::shared_ptr<gl::Shader> load_shader(const fs::path& id){
  // need to include args twice :///
  auto path = _shader_path/id;
  auto ptr = get<gl::Shader>(id.string(), path);

  _events->emit(event::Track{path.string(), [&ptr](auto){
        ptr->reload();

        if(ptr->is_loaded() && ptr->has_shader_program()){
          ptr->shader_program()->link();
        }
      }});

  return ptr;
}

std::shared_ptr<gl::Texture> load_texture(fs::path id){
  return nullptr;
}

} // namespace weq::memory::resource_manager
