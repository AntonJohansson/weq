#pragma once

#include <functional>
#include <imgui/imgui.h>

namespace entityx{class EventManager;}

namespace component{

struct ImGui{
  using Func = std::function<void(entityx::EventManager& e)>;
  ImGui(Func func)
    : _register_ui(func){
  }

  Func _register_ui;
};

}
