#include <weq/Window.hpp>
#include <weq/event/Window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <entityx/entityx.h>
#include <spdlog/spdlog.h>

/*
 *    Handle window-independent stuff such as GL-context (TODO move?) and GLFW-init.
 */

namespace{
  int _window_count = 0;

  void error_callback(int error, const char* desc){
    (void)error;
    spdlog::get("console")->error("GLFW error {}", desc);
  }

  void window_initialize(){
    if(_window_count == 0){
      spdlog::get("console")->info("Running GLFW {}", glfwGetVersionString());

      if(!glfwInit()){
        spdlog::get("console")->error("Failed to initialize GLFW!");
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

/*
 *    Window Impl.
 */

Window::Window(entityx::EventManager& events, WindowMode mode)
  : _events(events),
    _mode(mode),
    _x(0),
    _y(0),
    _width(1280),
    _height(720),
    _refresh_rate(60){
  window_initialize();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  _window = glfwCreateWindow(_width, _height, "WEQ", nullptr, nullptr);

  if(!_window){
    spdlog::get("console")->error("GLFW: Failed to create window!");
    window_terminate();
  }

  make_current();

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
    spdlog::get("console")->error("GLAD: Failed to initialize OpenGL context");
  }

  spdlog::get("console")->info("Using OpenGL {}.{}", GLVersion.major, GLVersion.minor);

  float aspect_ratio = static_cast<float>(_width)/static_cast<float>(_height);
  _events.emit(event::WindowUpdate(_width, _height, _refresh_rate, aspect_ratio));
}

Window::~Window(){
  glfwDestroyWindow(_window);
  glfwTerminate();
  window_terminate();
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::poll_events(){
  glfwPollEvents();
}

// Set stuff

void Window::set_size(int w, int h){
  _width = w;
  _height = h;
  glfwSetWindowSize(_window, _width, _height);
}

void Window::set_mode(WindowMode mode){
  _mode = mode;
  auto primary_monitor = monitor(0);
  auto vid_mode = glfwGetVideoMode(primary_monitor);

  switch(mode){
  case WindowMode::WINDOWED:
    glfwSetWindowMonitor(_window, nullptr, 0, 0, _width, _height, 0);
    break;
  case WindowMode::FULLSCREEN:
    glfwSetWindowMonitor(_window, primary_monitor, 0, 0, _width, _height, _refresh_rate);
    break;
  case WindowMode::WINDOWED_FULLSCREEN:
    glfwSetWindowMonitor(_window, primary_monitor, 0, 0, vid_mode->width, vid_mode->height, vid_mode->refreshRate);
    break;
  default:
    // TODO print actual window enum (convert to string?)
    spdlog::get("console")->error("Unrecognized window mode {}");
    break;
  }
}

void Window::set_cursor_mode(CursorMode mode){
  switch(mode){
  case CursorMode::NORMAL:
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case CursorMode::DISABLED:
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
  default:
    // TODO print actual window enum (convert to string?)
    spdlog::get("console")->error("Unrecognized window mode");
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
