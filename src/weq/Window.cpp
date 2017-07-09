#include <weq/Window.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(Mode mode)
  : _mode(mode),
    _x(0),
    _y(0),
    _w(640),
    _h(480),
    _refresh_rate(60){
  auto str = glfwGetVersionString();
  std::cout << "Running: " << str << std::endl;

  if(!glfwInit()){
    std::cerr << "GLFW failed to init\n";
  }

  _window = glfwCreateWindow(640, 480, "WEQ", nullptr, nullptr);

  if(!_window){
    Window::terminate();
    std::cerr << "GLFW window creation failed\n";
  }
}

Window::~Window(){
  glfwDestroyWindow(_window);
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

void Window::poll_events(){
  glfwPollEvents();
}

void Window::terminate(){
  glfwTerminate();
}

// Set stuff

void Window::set_size(int w, int h){
  _w = w;
  _h = h;
  glfwSetWindowSize(_window, _w, _h);
}

void Window::set_mode(Mode mode){
  _mode = mode;
  auto primary_monitor = glfwGetPrimaryMonitor();
  auto vid_mode = glfwGetVideoMode(primary_monitor);

  switch(mode){
  case Mode::WINDOWED:
    //glfwSetWindowMonitor(_window, nullptr, 0, 0, _w, _h, 0);
    break;
  case Mode::FULLSCREEN:
    //glfwSetWindowMonitor(_window, primary_monitor, 0, 0, _w, _h, _refresh_rate);
    break;
  case Mode::WINDOWED_FULLSCREEN:
    //glfwSetWindowMonitor(_window, primary_monitor, 0, 0, vid_mode->width, vid_mode->height, vid_mode->refreshRate);
    break;
  }
}

// Get stuff

bool Window::should_close(){
  return glfwWindowShouldClose(_window);
}

GLFWmonitor* Window::monitor(){
  auto monitor = glfwGetWindowMonitor(_window);
  return monitor;
}

