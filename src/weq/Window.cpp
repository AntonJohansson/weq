#include <weq/Window.hpp>
#include <weq/ecs/EventManager.hpp>
#include <weq/event/Window.hpp>

#include <weq/memory/ResourceManager.hpp>
#include <weq/vars/Vars.hpp>

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
////#include <entityx/entityx.h>
//#include <spdlog/spdlog.h>

namespace{
  bool glfw_initialized;
  bool glad_initialized;
  Var(bool, gl_debug_output, false);
  Var(bool, gl_cullface, true);
  Var(int,  gl_multisample, 0);
}

static std::string APIENTRY gl_error_source_to_string(GLenum source){
  switch(source){
  case GL_DEBUG_SOURCE_API:
    return "api"; break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "window system"; break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    return "shader compiler"; break;
  case GL_DEBUG_SOURCE_APPLICATION:
    return "shader compiler"; break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "third party"; break;
  case GL_DEBUG_SOURCE_OTHER:
    return "other"; break;
  default:
    return "unknown source";
  }
}

static std::string APIENTRY gl_error_type_to_string(GLenum type){
  switch(type){
  case GL_DEBUG_TYPE_ERROR:
    return "ERROR"; break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "undefined behaviour"; break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "deprecated behaviour"; break;
  case GL_DEBUG_TYPE_PORTABILITY:
    return "portability"; break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    return "performance"; break;
  case GL_DEBUG_TYPE_MARKER:
    return "marker"; break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    return "push group"; break;
  case GL_DEBUG_TYPE_POP_GROUP:
    return "pop group"; break;
  case GL_DEBUG_TYPE_OTHER:
    return "other"; break;
  default:
    return "unknown message type";
  };
}

static std::string APIENTRY gl_error_severity_to_string(GLenum severity){
  switch(severity){
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    return "notification"; break;
  case GL_DEBUG_SEVERITY_LOW:
    return "low"; break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    return "medium"; break;
  case GL_DEBUG_SEVERITY_HIGH:
    return "high"; break;
  default:
    return "unknown severity";
  };
}

static void APIENTRY gl_error_callback( GLenum source,
                        GLenum type,
                        GLuint id,
                        GLenum severity,
                        GLsizei length,
                        const GLchar* message,
                        const void* userParam ){
  (void)id;
  (void)length;
  (void)userParam;

  if(severity == GL_DEBUG_SEVERITY_NOTIFICATION)return;
  spdlog::get("console")->error("OpenGL debug message\nsource: {}\ntype: {}\nseverity: {}\nmessage: {}",
                                gl_error_source_to_string(source),
                                gl_error_type_to_string(type),
                                gl_error_severity_to_string(severity),
                                message);
}

namespace weq{




Window::Window(EventManager& events, std::string title, unsigned int width,
               unsigned int height, WindowMode mode)
  : _events(events),
    _x(0),
    _y(0),
    _width(width),
    _height(height),
    _refresh_rate(60),
    _mode(mode){

  // Load config file for window
  // Callback will never be called since hotloader system hasn't been created yet.
  // buffer events? (@TODO)
  memory::resource_manager::load_tweak_file("OpenGL.vars", [&](){
    });

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

  if(gl_multisample.var > 0){
		spdlog::get("console")->info("multisample enabled!");
    glfwWindowHint(GLFW_SAMPLES, gl_multisample.var);
  }

  if(_mode == WindowMode::WINDOWED){
    _window = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
  }else if(_mode == WindowMode::FULLSCREEN){
    _window = glfwCreateWindow(_width, _height, title.c_str(), glfwGetPrimaryMonitor(), nullptr);
  }else if(_mode == WindowMode::WINDOWED_FULLSCREEN){
    auto* video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwWindowHint(GLFW_RED_BITS,     video_mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS,   video_mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS,    video_mode->blueBits);
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
  (void)aspect_ratio;
  //_events.emit(event::WindowUpdate(_width, _height, _refresh_rate, aspect_ratio));

  //glEnable(GL_DEBUG_OUTPUT);
  //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
  //glDebugMessageCallback((GLDEBUGPROC) gl_error_callback, 0);

  if(gl_multisample.var > 0){
    glEnable(GL_MULTISAMPLE);
  }

  if(gl_cullface.var){
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }

  if(gl_debug_output.var){
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glDebugMessageCallback((GLDEBUGPROC) gl_error_callback, 0);
  }else{
    //glDisable(GL_DEBUG_OUTPUT);
  }
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
