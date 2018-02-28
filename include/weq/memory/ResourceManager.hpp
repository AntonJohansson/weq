#pragma once

//#include <weq/resource/Resource.hpp>
//
//#include <unordered_map>
//#include <type_traits>
//#include <memory>
//#include <string>
//
//#include <FreeImage.h>

//#include <entityx/entityx.h>
#include <weq/ecs/Fwd.hpp>

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
#endif

#include <string>
#include <memory>
#include <unordered_map>

// Forward declarations
namespace weq::gl{
class ShaderProgram;
class Shader;
class Texture;
class Cubemap;
}

namespace weq::memory{
class Resource;
}

//class ResourceManager{
//public:
//  ResourceManager(){
//    FreeImage_Initialise();
//  }
//
//  ~ResourceManager(){
//    FreeImage_DeInitialise();
//  }
//
//  template<typename T, typename... Args>
//  std::shared_ptr<T> get(std::string id, Args&&... args){
//    static_assert(std::is_base_of<Resource, T>::value, "T should inherit from resource");
//
//    auto& wp = _memory[id];
//    auto sp = wp.lock();
//
//    if(!sp){
//      wp = sp = std::make_shared<T>(id, std::forward<Args>(args)...);
//      sp->load();
//    }
//
//    return std::dynamic_pointer_cast<T>(sp);
//  }
//
//private:
//  std::unordered_map<std::string,
//                     std::weak_ptr<Resource>> _memory;
//};

namespace weq::memory::resource_manager{

//namespace ex = entityx;
namespace fs = std::experimental::filesystem;

extern std::unordered_map<std::string, std::weak_ptr<memory::Resource>> _memory;

void initialize(EventManager& events);
void shutdown();

template<typename T, typename... Args>
std::shared_ptr<T> get(const std::string& id, Args&&... args){
  static_assert(std::is_base_of<Resource, T>::value, "T should inherit from Resource");

  auto& wp = _memory[id];
  auto  sp = wp.lock();

  if(!sp){
    wp = sp = std::make_shared<T>(std::forward<Args>(args)...);
    sp->load();
  }

  return std::dynamic_pointer_cast<T>(sp);
}

std::shared_ptr<gl::ShaderProgram> load_shader_program(const fs::path& id);
std::shared_ptr<gl::Shader>        load_shader(const fs::path& id);
std::shared_ptr<gl::Texture>       load_texture(const fs::path& id);
std::shared_ptr<gl::Cubemap>       load_cubemap(const fs::path& front,
                                                const fs::path& back,
                                                const fs::path& up,
                                                const fs::path& down,
                                                const fs::path& left,
                                                const fs::path& right);


} // namespace weq::memory::resource_manager
