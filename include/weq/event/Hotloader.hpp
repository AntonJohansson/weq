#pragma once

//#include <string>
//#include <functional>

namespace weq::event{

using UpdateFunc = std::function<void(std::string)>;

struct Track{
  std::string path;
  std::function<void(std::string)> func;
};

} // weq::event
