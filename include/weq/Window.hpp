#pragma once

class GLFWwindow;
class GLFWmonitor;

enum class Mode{
  WINDOWED,
  FULLSCREEN,
  WINDOWED_FULLSCREEN
};

class Window{
public:
  Window(Mode mode = Mode::WINDOWED);
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
  int width(){return _w;}
  int height(){return _h;}
  int x_pos(){return _x;}
  int y_pos(){return _y;}
  int refresh_rate(){return _refresh_rate;}

private:
  /* Returns the current monitor for this window,
     only makes sense in the context of fullscreen
     windows */
  GLFWmonitor* current_monitor();
  /* Returns the monitor with a given index,
     0 = primary monitor */
  GLFWmonitor* monitor(int index);

  Mode _mode;
  int _x;
  int _y;
  int _w;
  int _h;
  int _refresh_rate;
  GLFWwindow* _window;
};
