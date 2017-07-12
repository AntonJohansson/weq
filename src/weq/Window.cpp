#include <weq/Window.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace{
  int _window_count = 0;

  void error_callback(int error, const char* desc){
    std::cerr << desc << std::endl;
  }

  void window_initialize(){
    if(_window_count == 0){
      auto str = glfwGetVersionString();
      std::cout << "Running GLFW: " << str << std::endl;

      if(!glfwInit()){
        std::cerr << "GLFW failed to init\n";
      }

      glfwSetErrorCallback(error_callback);
    }

    _window_count++;
  }

  void window_terminate(){
    _window_count--;

    if(_window_count == 0){
      glfwTerminate();
    }
  }
}

Window::Window(Mode mode)
  : _mode(mode),
    _x(0),
    _y(0),
    _w(640),
    _h(480),
    _refresh_rate(60){
  window_initialize();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  _window = glfwCreateWindow(640, 480, "WEQ", nullptr, nullptr);

  if(!_window){
    std::cerr << "GLFW window creation failed\n";
    window_terminate();
  }

  make_current();

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
    std::cerr << "Failed to initialize OpenGL context" << std::endl;
  }

  std::cout << "Using OpenGL " << GLVersion.major << "." << GLVersion.minor << std::endl;
}

Window::~Window(){
  glfwDestroyWindow(_window);
  glfwTerminate();
  //window_terminate();
}

// Do stuff

void Window::make_current(){
  glfwMakeContextCurrent(_window);
}

void Window::swap_buffers(){
  glfwSwapBuffers(_window);
}

void Window::close(){
  glfwSetWindowShouldClose(_window, true);
}

void Window::clear(float r, float g, float b, float a){
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Window::poll_events(){
  glfwPollEvents();
}

// Set stuff

void Window::set_size(int w, int h){
  _w = w;
  _h = h;
  glfwSetWindowSize(_window, _w, _h);
}

void Window::set_mode(Mode mode){
  _mode = mode;
  auto primary_monitor = monitor(0);
  auto vid_mode = glfwGetVideoMode(primary_monitor);

  switch(mode){
  case Mode::WINDOWED:
    glfwSetWindowMonitor(_window, nullptr, 0, 0, _w, _h, 0);
    break;
  case Mode::FULLSCREEN:
    glfwSetWindowMonitor(_window, primary_monitor, 0, 0, _w, _h, _refresh_rate);
    break;
  case Mode::WINDOWED_FULLSCREEN:
    glfwSetWindowMonitor(_window, primary_monitor, 0, 0, vid_mode->width, vid_mode->height, vid_mode->refreshRate);
    break;
  }
}

// Get stuff

bool Window::should_close(){
  return glfwWindowShouldClose(_window);
}

GLFWwindow* Window::current(){
  return glfwGetCurrentContext();
}

GLFWmonitor* Window::current_monitor(){
  auto monitor = glfwGetWindowMonitor(_window);
  return monitor;
}

GLFWmonitor* Window::monitor(int index){
  int count;
  auto** monitors = glfwGetMonitors(&count);

  return (index < count && index >= 0) ? monitors[index] : nullptr;
}

