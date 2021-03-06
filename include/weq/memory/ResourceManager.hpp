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
#include <weq/memory/Resource.hpp>
#include <weq/utility/HashMap.hpp>

#if __has_include(<filesystem>)
#include <filesystem>
#ifdef _WIN32
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


#include <string>
#include <memory>
//#include <unordered_map>
#include <functional>

// Forward declarations
namespace weq::gl{
class ShaderProgram;
class Shader;
class Texture;
class Cubemap;
}

namespace weq::memory::resource_manager{

extern HashMap<std::string, std::weak_ptr<memory::Resource>> _memory;

void initialize(EventManager& events);
void shutdown();

template<typename T, typename... Args>
std::shared_ptr<T> get(const std::string& id, Args&&... args){
  static_assert(std::is_base_of<Resource, T>::value, "T should inherit from Resource");

	auto& wp = _memory[id];	
	auto sp = wp.lock();

  if(!sp){
    sp = std::make_shared<T>(std::forward<Args>(args)...);
		wp = sp;
    sp->load();
  }

  return std::dynamic_pointer_cast<T>(sp);
}

void load_tweak_file(const fs::path& id, std::function<void()> on_load = nullptr);

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
