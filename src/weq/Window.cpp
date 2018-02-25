#include <weq/Window.hpp>
#include <weq/ecs/EventManager.hpp>
#include <weq/event/Window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <entityx/entityx.h>
#include <spdlog/spdlog.h>

namespace{
  bool glfw_initialized;
  bool glad_initialized;
}

namespace weq{

Window::Window(EventManager& events, std::string title, unsigned int width,
               unsigned int height, WindowMode mode)
  : _events(events),
    _x(0),
    _y(0),
    _width(width),
    _height(height),
    _mode(mode),
    _refresh_rate(60){

  // Initialize GLFW
  if(!glfw_initialized && !glfwInit()){
    spdlog::get("console")->error("Failed to initialize GLFW!");
    return;
  }else{
    glfw_initialized = true;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  if(_mode == WindowMode::WINDOWED){
    _window = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
  }else if(_mode == WindowMode::FULLSCREEN){
    _window = glfwCreateWindow(_width, _height, title.c_str(), glfwGetPrimaryMonitor(), nullptr);
  }else if(_mode == WindowMode::WINDOWED_FULLSCREEN){
    auto* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);

    _window = glfwCreateWindow(video_mode->width, video_mode->height, title.c_str(), glfwGetPrimaryMonitor(), nullptr);
  }

  if(!_window){
    spdlog::get("console")->error("GLFW: Failed to create window!");
    return;
  }

  make_current();

  // Initialize GLAD after the window context has been created.
  if (!glad_initialized && !gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
    spdlog::get("console")->error("GLAD: Failed to initialize OpenGL context");
    return;
  }else{
    glad_initialized = true;
  }

  spdlog::get("console")->info("Running GLFW {}", glfwGetVersionString());
  spdlog::get("console")->info("Using OpenGL {}.{}", GLVersion.major, GLVersion.minor);

  glfwSwapInterval(0); // Disable vsync

  // Check if texture lookup in vertex shader is supported.
  int max_units;
  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_units);
  spdlog::get("console")->info("MAX_VERTEX_TEXTURE_IMAGE_UNITS: {}", max_units);

  // Calculate aspect ratio and send an update event.
  float aspect_ratio = static_cast<float>(_width)/static_cast<float>(_height);
  //_events.emit(event::WindowUpdate(_width, _height, _refresh_rate, aspect_ratio));
}

Window::~Window(){
  glfwDestroyWindow(_window);
  glfwTerminate(); // TODO move this, doesn't work for multiple windows.
}

// Do stuff

void Window::make_current() const {
  glfwMakeContextCurrent(_window);
}

void Window::swap_buffers() const {
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
	break;
    // TODO print actual window enum (convert to string?)
    spdlog::get("console")->error("Unrecognized window mode");
  }
}

bool Window::should_close() const {
  return glfwWindowShouldClose(_window);
}

// Get stuff

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

} // namespace weq
