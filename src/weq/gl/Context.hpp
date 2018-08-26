#pragma once

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <spdlog/spdlog.h>

namespace weq::gl::context{

void error_callback(int error, const char* desc){
  (void)error;
  spdlog::get("console")->error("GLFW error {}", desc);
}

void initialize(){
  if(!glfwInit()){
    spdlog::get("console")->error("Failed to initialize GLFW!");
    return;
  }
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
    spdlog::get("console")->error("GLAD: Failed to initialize OpenGL context");
    return;
  }

  glfwSetErrorCallback(error_callback);

  spdlog::get("console")->info("Running GLFW {}", glfwGetVersionString());
  spdlog::get("console")->info("Using OpenGL {}.{}", GLVersion.major, GLVersion.minor);
}

void shutdown(){
  glfwTerminate();
}

} // namespace weq::gl::context
