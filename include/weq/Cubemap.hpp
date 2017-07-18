#pragma once

#include <weq/resource/Resource.hpp>
#include <vector>
#include <string>

/*
 * @TODO Merge Cubemap and Texture resources?
 */

class Cubemap : public Resource{
public:
  Cubemap(const std::string& id,
          const std::vector<std::string>& texture_ids);
  ~Cubemap();

  void load() override;
  void unload() override;

  void bind();

private:
  std::tuple<unsigned char*, unsigned int, unsigned int>
  load_texture(const std::string& id);

  std::vector<std::string> _texture_ids;
  unsigned int _cubemap;
};
