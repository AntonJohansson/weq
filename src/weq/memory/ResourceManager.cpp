#include <weq/memory/ResourceManager.hpp>
#include <weq/memory/Resource.hpp>

#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>

#include <weq/gl/Texture.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>

#include <weq/event/Hotloader.hpp>

#include <spdlog/spdlog.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
#endif

#include <unordered_map>

namespace weq::memory::resource_manager{

namespace{
  const fs::path _base_path = "../res";
  const fs::path _shader_path = _base_path/fs::path("/shaders");

  EventManager* _events;
}

std::unordered_map<std::string, std::weak_ptr<Resource>> _memory;

void initialize(EventManager& events){
  _events = &events;

  //FreeImage_Initialise();
}

void shutdown(){
  //FreeImage_DeInitialise();
}

std::shared_ptr<gl::ShaderProgram> load_shader_program(const fs::path& id){
  auto path = (_shader_path/id);
  auto vert_path = path.replace_extension(".vert");
  auto frag_path = path.replace_extension(".frag");
  auto vert = get<gl::Shader>(vert_path.filename().string(), vert_path);
  auto frag = get<gl::Shader>(frag_path.filename().string(), frag_path);

  auto ptr = get<gl::ShaderProgram>(id.string(), vert, frag);
  ptr->link();

  _events->emit(event::Track{vert_path.string(), [vert, ptr](auto){
        vert->reload();

        if(vert->is_loaded()){
          ptr->link();
        }
      }});
  _events->emit(event::Track{frag_path.string(), [frag, ptr](auto){
        frag->reload();

        if(frag->is_loaded()){
          ptr->link();
        }
      }});

  return ptr;
}

std::shared_ptr<gl::Shader> load_shader(const fs::path& id){
  // need to include args twice :///
  auto path = _shader_path/id;
  auto ptr = get<gl::Shader>(id.string(), path);

  return ptr;
}

std::shared_ptr<gl::Texture> load_texture(fs::path id){
  //i32 w, h, bpp;
  //u8 rgb = stbi_load(id.string().c_str(), &w, &h, &bpp, 3);
  return nullptr;
}

} // namespace weq::memory::resource_manager
