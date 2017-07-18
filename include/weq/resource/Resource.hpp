#pragma once

#include <string>

class Resource{
public:
  Resource(const std::string& id)
    : _id(id),
      _is_loaded(false),
      _is_managed(false){}

  virtual ~Resource(){}
  virtual void load() = 0;
  virtual void unload() = 0;
  virtual void reload(){unload();load();}

protected:
  static constexpr auto _resource_path = "/Users/antonjohansson/git/WaveEquationFDM/res/";
  std::string _id;
  bool _is_loaded;

private:
  bool _is_managed;
};
