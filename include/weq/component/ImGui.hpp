#pragma once

#include <weq/ecs/Fwd.hpp>
#include <imgui/imgui.h>
#include <functional>

namespace weq::component{

// Dirty ol' component taking a function that will be updated by the renderer
// once a frame. This allows for implementation of a immediate mode GUI such
// as ImGui.
struct ImGui{
  using Func = std::function<void(EventManager& e)>;

  ImGui(Func func)
    : _register_ui(func){
  }

  Func _register_ui;
};

} // namespace weq::component
