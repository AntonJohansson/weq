#pragma once

enum class InputAction{
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
