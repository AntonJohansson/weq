#pragma once

namespace weq::system{

enum class InputAction : int{
  SPAWN_PLANE_WAVE = 0,
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

enum class InputState : int{
  MOVE_FORWARD = 20,
  MOVE_BACK,
  MOVE_UP,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  CURSOR_DOWN,
  SPAWN_RAY,
  UNKNOWN,
};

enum class InputRange : int{
  CURSOR_X = 40,
  CURSOR_Y,
  CURSOR_DX,
  CURSOR_DY,
  CURSOR_SCROLL_X,
  CURSOR_SCROLL_Y,
  UNKNOWN,
};

} // namespace weq::system
