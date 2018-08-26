cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

#set(CXX "cc_args.py clang++")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# -cc1 flag for clang causes a lot of standard headers to not be found
# Xclang forces -emit-pch to be passed to -cc1
add_custom_target(pch ALL 
	clang++ -Xclang
	-emit-pch
	${CMAKE_CURRENT_SOURCE_DIR}/include/weq/precompiled.hpp 
	-o ${CMAKE_CURRENT_SOURCE_DIR}/include/weq/precompiled.hpp.pch
	-I${CMAKE_CURRENT_SOURCE_DIR}/external/unix/include
	-I${CMAKE_CURRENT_SOURCE_DIR}/include
	-std=c++1z)


project(weq CXX)

#add_subdirectory(tests)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

include(cotire)
#add_compile_options(-D GLFW_INCLUDE_NONE)
#add_compile_options(-D STB_IMAGE_IMPLEMENTATION)
if(WIN32)
  set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/x64)
  #set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/x32)
  set(library_path ${EXTERNAL_PATH}/lib)
  link_directories(${EXTERNAL_PATH}/lib)
elseif(UNIX AND NOT APPLE)
	#set(CMAKE_CXX_COMPILER "g++")
  set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/unix)
  set(library_path ${EXTERNAL_PATH}/lib)
  link_directories(${EXTERNAL_PATH}/lib)
  set(EXTRA_LIBS "stdc++fs" "pthread" "dl")

  add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
elseif(APPLE)
	#set(CMAKE_CXX_COMPILER "g++-7")
  set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/osx)
  set(library_path ${EXTERNAL_PATH}/lib)
  link_directories(${EXTERNAL_PATH}/lib)
  find_library(cocoa Cocoa REQUIRED)
  find_library(iokit IOKit REQUIRED)
  find_library(corevideo CoreVideo REQUIRED)
  set(EXTRA_LIBS "stdc++fs" ${cocoa} ${iokit} ${corevideo})

  add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
endif()

#find_library(glfw HINTS ${library_path} NAMES glfw3 REQUIRED)
#find_library(freeimage HINTS ${library_path} NAMES FreeImage REQUIRED)
#find_library(entityx HINTS ${library_path} NAMES entityx REQUIRED)
find_library(glfw HINTS ${library_path} NAMES glfw3 REQUIRED)
#find_package(glfw3 3.2 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_executable(weq
	src/weq/precompiled.cpp

  src/weq/Window.cpp
  src/weq/main.cpp
  src/weq/Application.cpp

	src/weq/utility/Profiler.cpp

  src/weq/gl/Framebuffer.cpp
  src/weq/gl/Shader.cpp
  src/weq/gl/ShaderProgram.cpp
  src/weq/gl/VertexArray.cpp
  src/weq/gl/VertexFormat.cpp
  src/weq/gl/Texture.cpp
  src/weq/gl/Cubemap.cpp

  src/weq/system/Camera.cpp
  src/weq/system/Hotloader.cpp
  src/weq/system/Input.cpp
  src/weq/system/InputAbstractionLayer.cpp
  src/weq/system/UserInterface.cpp
  src/weq/system/Renderer.cpp
  src/weq/system/WaveGPUSimulation.cpp
  src/weq/system/DebugDraw.cpp

  src/weq/vars/Vars.cpp

  src/weq/memory/ResourceManager.cpp

  src/weq/ecs/Event.cpp
  src/weq/ecs/System.cpp
  src/weq/ecs/Component.cpp

  src/glad/glad.cpp
  src/imgui/imgui.cpp
  src/imgui/imgui_demo.cpp
  src/imgui/imgui_draw.cpp
  src/imgui/imgui_impl_glfw_gl3.cpp)

if(WIN32)
  target_compile_options(weq PRIVATE /std:c++latest)
elseif(APPLE OR UNIX)
  # -stdlib=libc++ ?
	#target_compile_options(weq PRIVATE -O3 -fcolor-diagnostics -std=c++1z -Wall -Wextra -Wfatal-errors -g -pg)
	target_compile_options(weq PRIVATE 
		#-include-pch ${CMAKE_CURRENT_SOURCE_DIR}/include/weq/precompiled.hpp.pch 
		-fcolor-diagnostics -std=c++1z -Wall -Wextra -Wfatal-errors -g)
	#set_target_properties(weq PROPERTIES LINK_FLAGS "-pg")
endif()

set_target_properties(weq PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(weq PRIVATE GLFW_INCLUDE_NONE GLM_FORCE_RADIANS)
target_include_directories(weq PUBLIC
  $<BUILD_INTERFACE:${EXTERNAL_PATH}/include> # SHOULD ONLY BE ADDED ON WINDOWS
  #$<BUILD_INTERFACE:/usr/local/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE: include>)

#target_compile_definitions(weq PRIVATE FREEIMAGE_LIB)

target_link_libraries(weq
  ${glfw}
  ${OPENGL_gl_LIBRARY}
  ${EXTRA_LIBS}
  )

set_target_properties(weq PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "include/weq/precompiled.hpp")
cotire(weq)

