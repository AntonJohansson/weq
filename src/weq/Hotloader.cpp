//#include <weq/Hotloader.hpp>
//
//#include <spdlog/spdlog.h>
//
//#include <vector>
//#include <filesystem>
//
//#include <iostream>
//
//namespace weq::hotloader{
//namespace fs = std::experimental::filesystem;
//
//namespace{
//}
//
//bool should_update_file(fs::path path){
//  // Only update non-hidden files
//  if(fs::is_regular_file(path) && path.filename().string()[0] != '.'){
//    // Fetch write times
//    auto new_write_time = fs::last_write_time(path);
//
//    auto it = save_time_map.find(path.string());
//
//    if(it == save_time_map.end()){
//      // if the file has not been checked before, add it.
//      save_time_map.insert_or_assign(path.string(), new_write_time);
//    }
//    else if(it->second != new_write_time){
//      // Update saved write time
//      save_time_map.insert_or_assign(path.string(), new_write_time);
//      spdlog::get("console")->info("{} has been updated!", path.string());
//      return true;
//    }
//  }
//
//  return false;
//}
//
//void add(const std::string& path, UpdateFunc func){
//  path_map.insert({path, func});
//}
//
//void update(){
//}
//
//}
