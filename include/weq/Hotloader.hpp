#pragma once

#include <string>
#include <functional>

namespace weq::hotloader{

using UpdateFunc = std::function<void(std::string)>;

void add(const std::string& path, UpdateFunc func);
void update();

}
