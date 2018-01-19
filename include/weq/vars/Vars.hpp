#pragma once

#include <string>
#include <tuple>
#include <unordered_map>

namespace weq::vars{

extern std::unordered_map<std::string, std::tuple<void*, size_t, size_t>> var_map;

void read_file(const std::string& file);
void attach(const char* name, void* ptr, size_t size, size_t hash_code);

template<typename T>
struct VarType{
  VarType(T t, size_t s, const char* n)
    : var(t),
      size(s),
      name(n),
      hash_code(typeid(T).hash_code()){
    attach(name, &var, size, hash_code);
  }

  T var;
  size_t size;
  const char* name;
  size_t hash_code;
};

}

#define Var(TYPE, NAME, VALUE) \
  weq::vars::VarType<TYPE> NAME = {VALUE, sizeof(TYPE), #NAME};
