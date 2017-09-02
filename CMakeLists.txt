cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(CXX "cc_args.py clang++")

project(weq CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

if(WIN32)
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/x32/lib)
  set(library_path ${CMAKE_CURRENT_SOURCE_DIR}/external/x32/lib)
endif()

#find_library(glfw HINTS ${library_path} NAMES glfw3 REQUIRED)
find_library(freeimage HINTS ${library_path} NAMES FreeImage REQUIRED)
find_library(entityx HINTS ${library_path} NAMES entityx REQUIRED)
find_library(glfw HINTS ${library_path} NAMES glfw3 REQUIRED)
#find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(weq
  src/weq/Window.cpp
  src/weq/Texture.cpp
  src/weq/main.cpp
  src/weq/Cubemap.cpp
  src/weq/Application.cpp

  src/weq/gl/Framebuffer.cpp
  src/weq/gl/Shader.cpp
  src/weq/gl/ShaderProgram.cpp
  src/weq/gl/VertexArray.cpp
  src/weq/gl/VertexFormat.cpp

  src/weq/system/Camera.cpp
  src/weq/system/Input.cpp
  src/weq/system/InputAbstractionLayer.cpp
  src/weq/system/UserInterface.cpp
  src/weq/system/Renderer.cpp
  src/weq/system/WaveGPUSimulation.cpp
  src/weq/system/WaveSimulation.cpp

  src/glad/glad.cpp
  src/imgui/imgui.cpp
  src/imgui/imgui_demo.cpp
  src/imgui/imgui_draw.cpp
  src/imgui/imgui_impl_glfw_gl3.cpp)

if(WIN32)
  target_compile_options(weq PRIVATE /std:c++latest)
else()
  target_compile_options(weq PRIVATE -std=c++1z -Wall -Wextra -g)
endif()


set_target_properties(weq PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(weq PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/x32/include> # SHOULD ONLY BE ADDED ON WINDOWS
  $<BUILD_INTERFACE:/usr/local/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE: include>)

target_compile_definitions(weq PRIVATE FREEIMAGE_LIB)

target_link_libraries(weq
  ${glfw} ${freeimage} ${entityx}
  ${OPENGL_gl_LIBRARY}
  )
