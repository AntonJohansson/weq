#include <weq/utility/Profiler.hpp>

namespace weq::utility{
std::unordered_map<std::string, Profiler::time_entry> Profiler::entries{};
}
