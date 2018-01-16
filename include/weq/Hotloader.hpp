#pragma once

#include <string>

namespace weq::hotloader{

void add_directory(const std::string& path);
void add_file(const std::string& path);
void update();

}
