#pragma once

namespace event{

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

} // namespace event
