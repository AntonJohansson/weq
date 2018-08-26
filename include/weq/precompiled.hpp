#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <stb/stb_image.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext.hpp>

#include <xmmintrin.h>
#include <pmmintrin.h> // needed for hadd in inverse(...)
#include <smmintrin.h> // needed for blendv in transform_inverse(...)

#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <map>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <initializer_list>
#include <array>
#include <numeric>
#include <bitset>
#include <utility>
#include <queue>
#include <new>
#include <optional>
#include <thread>
#include <cstdint>
#include <functional>
#include <tuple>

#include <chrono> // needed for multiple dts
using namespace std::chrono_literals;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::duration;


#if __has_include(<filesystem>)
#include <filesystem>
#ifdef _WIN32
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

// Windows does this for some reason
#ifdef _WIN32
#undef near
#undef far
#endif
