#include <weq/memory/ResourceManager.hpp>
#include <weq/memory/Resource.hpp>

#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>

#include <weq/gl/Texture.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Cubemap.hpp>

#include <weq/vars/Vars.hpp>

#include <weq/event/Hotloader.hpp>

namespace weq::memory::resource_manager{

namespace{
	// operator/ won't work on linux for some reason
  const fs::path _base_path     = fs::path("../res");
  const fs::path _shader_path   = fs::path("../res/shaders");
  const fs::path _texture_path  = fs::path("../res/textures");
  //const fs::path _shader_path   = _base_path/fs::path("/shaders");
  //const fs::path _texture_path  = _base_path/fs::path("/textures");

  EventManager* _events;
}

HashMap<std::string, std::weak_ptr<Resource>> _memory;

void initialize(EventManager& events){
  _events = &events;
}

void shutdown(){
}





void load_tweak_file(const fs::path& id, std::function<void()> on_load){
  auto path = _base_path/id;
  vars::read_file(path.string());

  _events->emit(weq::event::Track{path.string(), [&](auto path){
        weq::vars::read_file(path);
        if(on_load)on_load();
      }});

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

std::shared_ptr<gl::Texture> load_texture(const fs::path& id){
  auto path = _texture_path/id;
  auto ptr = get<gl::Texture>(id.string(), path);
  //i32 w, h, bpp;
  //u8 rgb = stbi_load(id.string().c_str(), &w, &h, &bpp, 3);
  return ptr;
}

std::shared_ptr<gl::Cubemap> load_cubemap(const fs::path& front,
                                          const fs::path& back,
                                          const fs::path& up,
                                          const fs::path& down,
                                          const fs::path& left,
                                          const fs::path& right){
  auto path = front.filename();
  return get<gl::Cubemap>(path.string(), path,
                          std::array<fs::path, 6>{
                            //_texture_path/right,
                            //_texture_path/left,
                            //_texture_path/back,
                            //_texture_path/front,
                            //_texture_path/up,
                            //_texture_path/down,
 _texture_path/right,
 _texture_path/left,
 _texture_path/up,
 _texture_path/down,
 _texture_path/front,
 _texture_path/back,
                            });
}


} // namespace weq::memory::resource_manager
