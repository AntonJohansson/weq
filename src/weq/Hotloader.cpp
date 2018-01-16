#include <weq/Hotloader.hpp>

#include <spdlog>

#include <vector>
#include <filesystem>

namespace weq::hotloader{
namespace{
  std::vector<std::string> paths;
}

void add_directory(const std::string& path){
  paths.push_back(path);
}

void add_path(const std::string& path){
  paths.push_back(path);
}

void update(){
  for(auto& path : paths){
    if(std::filesystem::is_directory(path)){
      spdlog::get("console")->info("{} is a directory!", path);
    }else if(std::filesystem::is_regular_file(path)){
      spdlog::get("console")->info("{} is a regular old file!", path);
    }
  }
}

}
