#pragma once

#include <string>
#include <functional>

namespace weq::hotloader{

using UpdateFunc = std::function<void(std::string)>;

// Add a file or directory for tracking, specifying an update callback.
void add(const std::string& path, UpdateFunc func);
// Update files if necessary.
void update();

}
