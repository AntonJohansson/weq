#pragma once

#include <string>
#include <filesystem>

namespace weq::memory{

namespace fs = std::experimental::filesystem;

enum class ResourceType{
  FILE,
  MEMORY,
};

class Resource{
public:
  Resource(const fs::path& path = "")
    : _path(path),
      _is_loaded(false),
      _type((path == "") ? ResourceType::MEMORY : ResourceType::FILE){}

  virtual ~Resource(){}

  virtual void load() = 0;
  virtual void unload() = 0;
  virtual void reload(){unload();load();}

  const fs::path& path(){return _path;}

  bool is_loaded(){return _is_loaded;}

protected:
  fs::path _path;
  bool _is_loaded;
  ResourceType _type;

private:
  friend class ResourceManager;

  // Set the path where all resources will be located.
  //void set_resource_path(const std::string& path){
  //  _resource_path = &path;
  //}
};

} // namespace weq::memory
