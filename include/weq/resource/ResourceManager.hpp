#pragma once

#include <weq/resource/Resource.hpp>

#include <unordered_map>
#include <type_traits>
#include <memory>
#include <string>

#include <FreeImage.h>

class ResourceManager{
public:
  ResourceManager(){
    FreeImage_Initialise();
  }

  ~ResourceManager(){
    FreeImage_DeInitialise();
  }

  template<typename T, typename... Args>
  std::shared_ptr<T> get(std::string id, Args&&... args){
    static_assert(std::is_base_of<Resource, T>::value, "T should inherit from resource");

    auto& wp = _memory[id];
    auto sp = wp.lock();

    if(!sp){
      wp = sp = std::make_shared<T>(id, std::forward<Args>(args)...);
      sp->load();
    }

    return std::dynamic_pointer_cast<T>(sp);
  }

private:
  std::unordered_map<std::string,
                     std::weak_ptr<Resource>> _memory;
};
