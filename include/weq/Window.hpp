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
  static void poll_events();
  static void terminate();

  void set_size(int w, int h);
  void set_mode(Mode mode);

  bool should_close();
  GLFWmonitor* monitor();
  int width(){return _w;}
  int height(){return _h;}
  int x_pos(){return _x;}
  int y_pos(){return _y;}
  int refresh_rate(){return _refresh_rate;}

private:
  Mode _mode;
  int _x;
  int _y;
  int _w;
  int _h;
  int _refresh_rate;
  GLFWwindow* _window;
};
