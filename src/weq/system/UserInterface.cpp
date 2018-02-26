#include <weq/system/UserInterface.hpp>

#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>

#include <weq/component/ImGui.hpp>

#include <weq/event/Input.hpp>
#include <weq/event/Window.hpp>

#include <spdlog/spdlog.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <vector>

namespace{
  unsigned int window_height;
  unsigned int window_width;
  std::vector<ImVec2> menu_pos;
  std::vector<ImVec2> menu_size;
}

namespace weq::system{

UserInterface::UserInterface(){}
UserInterface::~UserInterface(){}

void UserInterface::configure(EventManager& events){
  System<UserInterface>::configure(events);
  set_timestep(8ms);
  events.subscribe<event::ActiveInput, 0>(*this); // userinterface while have high prio on input events
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

  for(std::string window : {"Menu", "Debug"}){
    ImGui::Begin(window.c_str());
    auto pos  = ImGui::GetWindowPos();
    auto size = ImGui::GetWindowSize();
    menu_pos.push_back(pos);
    menu_size.push_back(size);
    ImGui::End();
  }


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
  // @TODO hardcoded window width
  // @TODO windows are curretnly hardcoded

  for(int i = 0; i < menu_pos.size(); i++){
    auto pos  = menu_pos[i];
    auto size = menu_size[i];

    if(event.has(InputRange::CURSOR_X) && event.has(InputRange::CURSOR_Y)){
      float x = event.ranges.at(InputRange::CURSOR_X);
      float y = event.ranges.at(InputRange::CURSOR_Y);

      x = window_width/2 + x*window_width/2;
      y = window_height - (window_height/2 + y*window_height/2); // ImGui inverted y-axis

      //spdlog::get("console")->info("{}, {}", pos.x, pos.y);
      //spdlog::get("console")->info("{}, {}", x, y);

      if(x >= pos.x && x <= (pos.x + size.x) &&
         y >= pos.y && y <= (pos.y + size.y)){
        spdlog::get("console")->info("in menu!");
        alpha = 1.0f;
        return;
      }
    }
  }
  //spdlog::get("console")->info("");

  menu_pos.clear();
  menu_size.clear();



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
