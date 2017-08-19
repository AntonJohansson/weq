#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

namespace gl::context{

void initialize(){
  if(!glfwInit()){
    spdlog::get("console")->error("Failed to initialize GLFW!");
  }
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
    spdlog::get("console")->error("GLAD: Failed to initialize OpenGL context");
  }
}

}
