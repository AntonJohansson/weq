#pragma once

#include <weq/Window.hpp>
//#include <memory>

namespace weq::event{

// Event sent out by the WindowSystem (which does not exist. Yet.)
// contains the data that has been updated such as window size,
// refresh rate, etc..
struct WindowUpdate{
  WindowUpdate(unsigned int w, unsigned int h, unsigned int refresh_rate, float aspect_ratio)
    : width(w),
      height(h),
      refresh_rate(refresh_rate),
      aspect_ratio(aspect_ratio){
  }

  unsigned int width;
  unsigned int height;
  unsigned int refresh_rate;
  float aspect_ratio;
};

// Event sent out when the active window is changed
struct ActiveWindow{
  ActiveWindow(std::shared_ptr<Window> w)
    : window(w){}

  std::shared_ptr<Window> window;
};

} // namespace weq::event
