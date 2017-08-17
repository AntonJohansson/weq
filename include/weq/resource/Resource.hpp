#pragma once

#include <string>

enum class ResourceType{
  FILE,
  MEMORY,
};

class Resource{
public:
  Resource(const std::string& id, ResourceType type)
    : _id(id),
      _is_loaded(false),
      _type(type){}

  virtual ~Resource(){}

  virtual void load() = 0;
  virtual void unload() = 0;
  virtual void reload(){unload();load();}
  const std::string& id(){return _id;}

protected:
  //static constexpr auto _resource_path = "/Users/antonjohansson/git/WaveEquationFDM/res/";
	static constexpr auto _resource_path = "C:/Users/Anton Johansson/Desktop/weq";
  std::string _id;
  bool _is_loaded;
  ResourceType _type;

private:
  friend class ResourceManager;

  // Set the path where all resources will be located.
  //void set_resource_path(const std::string& path){
  //  _resource_path = &path;
  //}
};
