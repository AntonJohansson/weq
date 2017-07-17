#pragma once

enum class InputAction{
  SPAWN_PLANE_WAVE,
  CLEAR,
  SINGLE_SLIT,
  DOUBLE_SLIT,
  INCREASE_D,
  DECREASE_D,
  INCREASE_B,
  DECREASE_B,
  UNKNOWN,
};

enum class InputState{
  MOVE_FORWARD,
  MOVE_BACK,
  MOVE_UP,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  UNKNOWN,
};

enum class InputRange{
  CURSOR_X,
  CURSOR_Y,
  UNKNOWN,
};
