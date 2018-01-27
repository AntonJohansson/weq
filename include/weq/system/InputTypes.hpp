#pragma once

namespace weq::system{

enum class InputAction{
  SPAWN_PLANE_WAVE,
  SPAWN_WAVELET,
  UPDATE_COLOR,
  CLEAR,
  SINGLE_SLIT,
  DOUBLE_SLIT,
  INCREASE_D,
  DECREASE_D,
  INCREASE_B,
  DECREASE_B,
  INCREASE_C,
  DECREASE_C,
  INCREASE_T,
  DECREASE_T,
  RENDER_WIREFRAME,
  RENDER_SOLID,
  TOGGLE_REFLECT,
  UNKNOWN,
};

enum class InputState{
  MOVE_FORWARD,
  MOVE_BACK,
  MOVE_UP,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  CURSOR_DOWN,
  SPAWN_RAY,
  UNKNOWN,
};

enum class InputRange{
  CURSOR_X,
  CURSOR_Y,
  CURSOR_DX,
  CURSOR_DY,
  CURSOR_SCROLL_X,
  CURSOR_SCROLL_Y,
  UNKNOWN,
};

} // namespace weq::system
