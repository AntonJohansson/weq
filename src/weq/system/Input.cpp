#include <weq/system/Input.hpp>
#include <weq/system/InputAbstractionLayer.hpp>
#include <weq/system/InputRaw.hpp>
#include <weq/system/InputTypes.hpp>
#include <weq/system/InputContext.hpp>
#include <weq/event/Window.hpp>
#include <weq/Window.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <initializer_list>

// TODO read input from config file instead of hard coded.

namespace{
  unsigned int window_width;
  unsigned int window_height;
}

namespace weq::system{

namespace{
  InputAbstractionLayer* _ial;

  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    (void)window;
    (void)scancode;
    (void)mods;
    (void)action;
    _ial->register_key(key, action, mods);
    // Connect to ImGui
    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
  }

  static void mouse_callback(GLFWwindow* window, double x, double y){
    (void)window;
    (void)x;
    (void)y;
    _ial->register_mouse(x, y, window_width, window_height); 
  }

  static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    _ial->register_key(button, action, mods);
    // Connect to ImGui
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
  }

  static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    // Connect to ImGui
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
    _ial->register_scroll(xoffset, yoffset);
  }

  static void character_callback(GLFWwindow* window, unsigned int codepoint){
    (void)window;
    (void)codepoint;
    // Connect to ImGui
    ImGui_ImplGlfwGL3_CharCallback(window, codepoint);
  }

  static void charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods){
    (void)window;
    (void)codepoint;
    (void)mods;
  }
}

void Input::configure(ex::EventManager& events){
  (void)events;

  // Events
  events.subscribe<event::ActiveWindow>(*this);

  // set the context to the current one
  // @TODO easy way to switch between contexts
  _window_context = Window::current();

  // Could I loop through all the contexts and force them to this
  // callback?
  glfwSetKeyCallback(_window_context, key_callback);
  glfwSetCursorPosCallback(_window_context, mouse_callback);
  glfwSetMouseButtonCallback(_window_context, mouse_button_callback);
  glfwSetScrollCallback(_window_context, scroll_callback);
  glfwSetCharCallback(_window_context, character_callback);
  //glfwSetCharModsCallback(_window_context, charmods_callback);

  // Setup ImGui binding
  // TODO move?
  ImGui_ImplGlfwGL3_Init(_window_context, false);

  // Inputsmaps
  InputMap<int, InputAction> action_map = {
    {raw::Key::E, InputAction::SPAWN_PLANE_WAVE},
    {raw::Key::Q, InputAction::SPAWN_WAVELET},
    {raw::Key::C, InputAction::CLEAR},
    {raw::Key::_2,InputAction::DOUBLE_SLIT},
    {raw::Key::I, InputAction::INCREASE_D},
    {raw::Key::K, InputAction::DECREASE_D},
    {raw::Key::U, InputAction::INCREASE_B},
    {raw::Key::J, InputAction::DECREASE_B},
    {raw::Key::Y, InputAction::INCREASE_C},
    {raw::Key::H, InputAction::DECREASE_C},
    {raw::Key::O, InputAction::INCREASE_T},
    {raw::Key::L, InputAction::DECREASE_T},
    {raw::Key::R, InputAction::RENDER_WIREFRAME},
    {raw::Key::F, InputAction::RENDER_SOLID},
    {raw::Key::V, InputAction::UPDATE_COLOR},
    {raw::Key::B, InputAction::TOGGLE_REFLECT}
  };

  InputMap<int, InputState> state_map = {
    {raw::Key::W, InputState::MOVE_FORWARD},
    {raw::Key::A, InputState::MOVE_LEFT},
    {raw::Key::S, InputState::MOVE_BACK},
    {raw::Key::D, InputState::MOVE_RIGHT},
    {raw::Key::LEFT_SHIFT, InputState::MOVE_UP},
    {raw::Key::LEFT_CONTROL, InputState::MOVE_DOWN},
    {raw::Mouse::BUTTON_LEFT, InputState::CURSOR_DOWN}
  };

  InputMap<int, InputRange> range_map = {
    {raw::Axes::MOUSE_X, InputRange::CURSOR_X},
    {raw::Axes::MOUSE_Y, InputRange::CURSOR_Y},
    {raw::Axes::MOUSE_SCROLL_X, InputRange::CURSOR_SCROLL_X},
    {raw::Axes::MOUSE_SCROLL_Y, InputRange::CURSOR_SCROLL_Y}
  };

  // Have to be explicit with initializer list when dealing with perferctly
  // forwarded arguments.
  auto context = std::make_shared<InputContext>(action_map,
                                                state_map,
                                                range_map);

  _ial = new InputAbstractionLayer(context); // hmmm nope
}

void Input::update(ex::EntityManager& entities,
            ex::EventManager& events,
            ex::TimeDelta dt){
  (void) entities;
  (void) dt;

  glfwPollEvents();
  events.emit(_ial->get_active());
  _ial->clear();
}

void Input::receive(const event::ActiveWindow& event){
  window_width = event.window->width();
  window_height = event.window->height();
}

}
