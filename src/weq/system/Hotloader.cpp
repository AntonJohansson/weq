#include <weq/system/Hotloader.hpp>

#include <weq/ecs/System.hpp>
#include <weq/ecs/EventManager.hpp>

#include <weq/event/Hotloader.hpp>

#include <spdlog/spdlog.h>

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
#endif

#include <unordered_map>

namespace weq::system{

namespace fs = std::experimental::filesystem;

namespace{
  std::unordered_map<std::string, event::UpdateFunc> path_map;
  std::unordered_map<std::string, fs::file_time_type> save_time_map;

  bool should_update_file(fs::path path){
    // Only update non-hidden files
    if(fs::is_regular_file(path) && path.filename().string()[0] != '.'){
      // Fetch write times
      auto new_write_time = fs::last_write_time(path);

      auto it = save_time_map.find(path.string());

      if(it == save_time_map.end()){
        // if the file has not been checked before, add it.
        save_time_map.insert_or_assign(path.string(), new_write_time);
      }
      else if(it->second != new_write_time){
        // Update saved write time
        save_time_map.insert_or_assign(path.string(), new_write_time);
        spdlog::get("console")->info("{} has been updated!", path.string());
        return true;
      }
    }

    return false;
  }
}

Hotloader::Hotloader(){
}

Hotloader::~Hotloader(){
}

void Hotloader::configure(EventManager& events){
  events.subscribe<event::Track>(*this);
}

void Hotloader::update(EntityManager& entities,
                       EventManager& events,
                       f32 dt){
  for(auto& [path, func] : path_map){
    // Directories, check files in dir recusively.
    if(fs::is_directory(path)){
      for(auto& entry : fs::recursive_directory_iterator(path)){
        if(should_update_file(entry.path())){
          func(entry.path().string());
        }
      }
    }else if(should_update_file(path)){
      // Handle regular old files
      func(path);
    }
  }
}


void Hotloader::receive(const event::Track& event){
  path_map.insert({event.path, event.func});
}

} // weq::system
