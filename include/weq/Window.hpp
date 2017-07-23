#pragma once

struct GLFWwindow;
struct GLFWmonitor;

namespace entityx{class EventManager;}

enum class Mode{
  WINDOWED,
  FULLSCREEN,
  WINDOWED_FULLSCREEN
};

class Window{
public:
  Window(entityx::EventManager& events, Mode mode = Mode::WINDOWED);
  ~Window();

  void make_current();
  void swap_buffers();
  void close();
  void clear(float r, float g, float b, float a = 1.0f);
  static void poll_events();

  void set_size(int w, int h);
  void set_mode(Mode mode);

  bool should_close();
  static GLFWwindow* current();
  unsigned int width(){return _width;}
  unsigned int height(){return _height;}
  unsigned int x_pos(){return _x;}
  unsigned int y_pos(){return _y;}
  unsigned int refresh_rate(){return _refresh_rate;}

private:
  /* Returns the current monitor for this window,
     only makes sense in the context of fullscreen
     windows */
  GLFWmonitor* current_monitor();
  /* Returns the monitor with a given index,
     0 = primary monitor */
  GLFWmonitor* monitor(int index);

  entityx::EventManager& _events;
  Mode _mode;
  unsigned int _x;
  unsigned int _y;
  unsigned int _width;
  unsigned int _height;
  unsigned int _refresh_rate;
  GLFWwindow* _window;
};
