#include <weq/system/UserInterface.hpp>

#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>

#include <weq/component/ImGui.hpp>

#include <weq/event/Input.hpp>
#include <weq/event/Window.hpp>

#include <spdlog/spdlog.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

namespace{
  ImVec2 menu_pos;
  ImVec2 menu_size;
  unsigned int window_width;
  unsigned int window_height;
}

namespace weq::system{

UserInterface::UserInterface(){}
UserInterface::~UserInterface(){}

void UserInterface::configure(EventManager& events){
  System<UserInterface>::configure(events);
  set_timestep(8ms);
  events.subscribe<event::ActiveInput>(*this);
  events.subscribe<event::ActiveWindow>(*this);
}

void UserInterface::update(EntityManager& entities,
                           EventManager& events,
                           f32 dt){
  ImGui_ImplGlfwGL3_NewFrame();

  //ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(20/255.f, 20/255.f, 20/255.f, 1.0f));
  //ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(40/255.f, 40/255.f, 40/255.f, 1.0f));
  //ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(135/255.f, 135/255.f, 135/255.f, 1.0f));
  //ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(20/255.f, 20/255.f, 20/255.f, 1.0f));
  //ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(40/255.f, 40/255.f, 40/255.f, 1.0f));
  //ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(135/255.f, 135/255.f, 135/255.f, 1.0f));

  //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

  entities.each<component::ImGui>([dt, &events](EntityId e, component::ImGui& i){
      if(i._register_ui)i._register_ui(events);
    });

  // Get position and size of menu
  // TODO this couples the "menu" and render system, which is not nice.
  //ImGui::Begin("Menu");
  //menu_pos = ImGui::GetWindowPos();
  //menu_size = ImGui::GetWindowSize();
  //ImGui::End();

  //ImGui::PopStyleVar();
  //ImGui::PopStyleColor(6);

  // Render is being called by the render system to ensure correct draw order.
  //ImGui::Render();
}

void UserInterface::receive(const event::ActiveInput& event){
  // TODO hardcoded window size.
  // Have to cast away const, since entityx doesn't allow for non-const events.
  //auto& non_const_event = const_cast<event::ActiveInput&>(event);

  //// Set cursor mode if the mouse is not in the menu => mouse will not be captured in menu.
  //if(non_const_event.has(InputRange::CURSOR_X)
  //   && non_const_event.has(InputRange::CURSOR_Y)){
  //  float x = non_const_event.ranges.at(InputRange::CURSOR_X);
  //  float y = non_const_event.ranges.at(InputRange::CURSOR_Y);

  //  x = x*window_width;
  //  y = y*window_height;

  //  if((x >= menu_pos.x && x <= (menu_pos.x + menu_size.x)) &&
  //     (y >= menu_pos.y && y <= (menu_pos.y + menu_size.y))){
  //    non_const_event.consume(InputState::CURSOR_DOWN); // consume the cursor down state if it exists.
  //  }
  //  //spdlog::get("console")->info("{}, {} - {}, {}", menu_pos.x, menu_pos.y, menu_size.x, menu_size.y);
  //}
}

void UserInterface::receive(const event::ActiveWindow& event){
  window_width = event.window->width();
  window_height = event.window->height();
}

}
