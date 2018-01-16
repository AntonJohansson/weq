#include <weq/Hotloader.hpp>

#include <spdlog/spdlog.h>

#include <vector>
#include <filesystem>

#include <iostream>

namespace weq::hotloader{
namespace fs = std::experimental::filesystem;

namespace{
  std::unordered_map<std::string, UpdateFunc> path_map;
  std::unordered_map<std::string, fs::file_time_type> save_time_map;
}

bool should_update_file(fs::path path){
  if(fs::is_regular_file(path) && path.filename().string()[0] != '.'){
    auto new_write_time = fs::last_write_time(path);

    auto it = save_time_map.find(path.string());

    if(it == save_time_map.end() || it->second != new_write_time){
      save_time_map.insert_or_assign(path.string(), new_write_time);
      spdlog::get("console")->info("{} has been updated!", path.string());
      return true;
    }
  }

  return false;
}

void add(const std::string& path, UpdateFunc func){
  path_map.insert({path, func});
}

void update(){
  for(auto& [path, func] : path_map){
    if(fs::is_directory(path)){
      for(auto& entry : fs::recursive_directory_iterator(path)){
        if(should_update_file(entry.path())){
          func(entry.path().string());
        }
      }
    }else if(fs::is_regular_file(path)){
      if(should_update_file(path)){
        func(path);
      }
    }
  }
}

}
