#pragma once

class Resource{
public:
  virtual ~Resource(){}
  virtual void load();
  virtual void reload();

private:
  bool _is_loaded;
};
