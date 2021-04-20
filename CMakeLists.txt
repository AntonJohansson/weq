cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(WEQ_INCLUDE_DIRS "")
add_subdirectory(third_party)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
message(STATUS ${THIRD_PARTY_INCLUDE_DIRS})
message(STATUS ${THIRD_PARTY_LIBS})

project(weq CXX)

include(cotire)

if(WIN32)
	set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/x64)
	set(library_path ${EXTERNAL_PATH}/lib)
	link_directories(${EXTERNAL_PATH}/lib)
elseif(UNIX AND NOT APPLE)
	set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/unix)
	set(library_path ${EXTERNAL_PATH}/lib)
	link_directories(${EXTERNAL_PATH}/lib)
	set(EXTRA_LIBS "stdc++fs" "pthread" "dl")

	add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
elseif(APPLE)
	set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/osx)
	set(library_path ${EXTERNAL_PATH}/lib)
	link_directories(${EXTERNAL_PATH}/lib)
	find_library(cocoa Cocoa REQUIRED)
	find_library(iokit IOKit REQUIRED)
	find_library(corevideo CoreVideo REQUIRED)
	set(EXTRA_LIBS "stdc++fs" ${cocoa} ${iokit} ${corevideo})

	add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
endif()

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(WEQ_SOURCES
	src/weq/precompiled.cpp

	src/weq/Window.cpp
	src/weq/main.cpp
	src/weq/Application.cpp

	src/weq/utility/Profiler.cpp

	src/weq/gl/ErrorHandler.cpp
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
	src/weq/system/UserInterface.cpp
	src/weq/system/Renderer.cpp
	src/weq/system/WaveGPUSimulation.cpp
	src/weq/system/DebugDraw.cpp

	src/weq/vars/Vars.cpp

	src/weq/memory/ResourceManager.cpp

	src/weq/imgui_impl_glfw_gl3.cpp

	src/weq/ecs/Event.cpp
	src/weq/ecs/System.cpp
	src/weq/ecs/Component.cpp)

add_executable(weq ${WEQ_SOURCES})

if(WIN32)
	target_compile_options(weq PRIVATE /std:c++latest)
elseif(APPLE OR UNIX)
	target_compile_options(weq PRIVATE
		-std=c++1z -Wall -Wextra -Wfatal-errors -g)
endif()

#set_target_properties(weq PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(weq PRIVATE GLFW_INCLUDE_NONE GLM_FORCE_RADIANS)
target_include_directories(weq PUBLIC
  $<BUILD_INTERFACE:${EXTERNAL_PATH}/include> # SHOULD ONLY BE ADDED ON WINDOWS
	$<BUILD_INTERFACE: ${THIRD_PARTY_INCLUDE_DIRS}
	$<BUILD_INTERFACE: ${WEQ_INCLUDE_DIRS}
  	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  	$<INSTALL_INTERFACE: include>)

target_link_libraries(weq
	${THIRD_PARTY_LIBS}
	${OPENGL_gl_LIBRARY}
	${EXTRA_LIBS})

set_target_properties(weq PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "include/weq/precompiled.hpp")
cotire(weq)
