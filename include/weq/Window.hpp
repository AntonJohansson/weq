#pragma once

#include <weq/ecs/Fwd.hpp>
#include <string>

// Defined in <glfw/glfw.h>
struct GLFWwindow;
struct GLFWmonitor;

namespace weq{

// Enum describing the possible window modes
enum class WindowMode{
  WINDOWED,
  FULLSCREEN,
  WINDOWED_FULLSCREEN
};

// Enum describing the possible mouse cursor modes.
// NORMAL = visable,
// DISABLED = not visible and kept at the center of the window.
enum class CursorMode{
  NORMAL,
  DISABLED,
};

// Class for describing a OpenGL window as provided by GLFW
// TODO add customization (vsync etc.).
class Window {
public:
  // Creates a new window with the given window mode.
  // TODO remove event manager and forward events from renderer.
  // Also requests a 4.1 OpenGL context if one does not already
  // exist.
  Window(EventManager& events, std::string title, unsigned int width,
         unsigned int height, WindowMode mode = WindowMode::WINDOWED);

  // Destorys the current window and all of it's resources.
  // Terminates GLFW if and only if the current window is the last one.
  ~Window();

  // Makes the OpenGL context of this window current on the calling thread.
  // A context can only be made current on a single thread at a time.
  void make_current() const;

  // Swaps the front and back buffers of the window.
  void swap_buffers() const;

  void close();

  // Clears the window context with the given color.
  void clear(float r, float g, float b, float a = 1.0f);

  // Poll ALL window and OS events.
  static void poll_events();

  // Sets the window and framebuffer size.
  void set_size(int w, int h);

  // Changes the current window mode. Possible modes are:
  // WindowMode::WINDOWED,
  // WindowMode::FULLSCREEN,
  // WindowMode::WINDOWED_FULLSCREEN,
  void set_mode(WindowMode mode);

  // Sets the mode of the mouse cursor. Possible modes are:
  // CursorMode::NORMAL,
  // CursorMode::DISABLED - which hides the mouse cursor and continously sets it's
  //                        position to (0,0).
  void set_cursor_mode(CursorMode mode);

  // Returns wheter or not the current window has received a "close" event
  // TODO remove. Handled by events.
  bool should_close() const;

  // Returns the current OpenGL context for the given thread.
  static GLFWwindow* current();

  // Returns width and height of the window.
  unsigned int width()  const {return _width;}
  unsigned int height() const {return _height;}
  float aspect_ratio() const {return static_cast<float>(_width)/static_cast<float>(_height);}

  // Returns the (x,y) position of the window on the current monitor.
  // TODO combine.
  unsigned int x_pos()  const {return _x;}
  unsigned int y_pos()  const {return _y;}

  // Returns the specified refresh rate of the window.
  unsigned int refresh_rate() const {return _refresh_rate;}

private:
  // Returns the current monitor for this window,
  // only makes sense in the context of fullscreen
  // windows.
  GLFWmonitor* current_monitor();
  // Returns the monitor with a given index,
  // 0 = primary monitor.
  GLFWmonitor* monitor(int index);

  EventManager& _events;
  unsigned int _x;
  unsigned int _y;
  unsigned int _width;
  unsigned int _height;
  unsigned int _refresh_rate;
  WindowMode _mode;
  GLFWwindow* _window;
};

} // namespace weq
