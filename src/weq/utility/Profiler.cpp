#include <weq/utility/Profiler.hpp>

namespace utility{
std::unordered_map<std::string, Profiler::time_entry> Profiler::entries{};
}
