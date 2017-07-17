#include <weq/system/Input.hpp>
#include <weq/system/InputAbstractionLayer.hpp>
#include <weq/Window.hpp>
#include <weq/system/InputRaw.hpp>

#include <GLFW/glfw3.h>

#include <string>
#include <deque>

namespace weq::system{

namespace{
  InputAbstractionLayer* _ial;

  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    (void)window;
    (void)scancode;
    (void)mods;
    (void)action;
    _ial->register_key(key, action, mods);
  }

  static void mouse_callback(GLFWwindow* window, double x, double y){
    (void)window;
    (void)x;
    (void)y;
    _ial->register_mouse(x, y, 640, 480);
  }

  static void character_callback(GLFWwindow* window, unsigned int codepoint){
    (void)window;
    (void)codepoint;
  }

  static void charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods){
    (void)window;
    (void)codepoint;
    (void)mods;
  }
}

void Input::configure(ex::EventManager& events){
  (void)events;

  // set the context to the current one
  // @TODO easy way to switch between contexts
  _window_context = Window::current();

  // Could I loop through all the contexts and force them to this
  // callback?
  glfwSetKeyCallback(_window_context, key_callback);
  glfwSetCursorPosCallback(_window_context, mouse_callback);
  glfwSetCharCallback(_window_context, character_callback);
  glfwSetCharModsCallback(_window_context, charmods_callback);

  glfwSetInputMode(_window_context, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  _ial = new InputAbstractionLayer({
      {
        {raw::Key::E, InputAction::SPAWN_PLANE_WAVE},
        {raw::Key::C, InputAction::CLEAR},
        {raw::Key::_1, InputAction::SINGLE_SLIT},
        {raw::Key::_1, InputAction::SINGLE_SLIT},
        {raw::Key::_2, InputAction::DOUBLE_SLIT},
        {raw::Key::I, InputAction::INCREASE_D},
        {raw::Key::K, InputAction::DECREASE_D},
        {raw::Key::U, InputAction::INCREASE_B},
        {raw::Key::J, InputAction::DECREASE_B},
      },
      {
        {raw::Key::W, InputState::MOVE_FORWARD},
        {raw::Key::A, InputState::MOVE_LEFT},
        {raw::Key::S, InputState::MOVE_BACK},
        {raw::Key::D, InputState::MOVE_RIGHT},
        {raw::Key::LEFT_SHIFT, InputState::MOVE_UP},
        {raw::Key::LEFT_CONTROL, InputState::MOVE_DOWN}
      },
      {
        {raw::Axes::MOUSE_X, InputRange::CURSOR_X},
        {raw::Axes::MOUSE_Y, InputRange::CURSOR_Y}
      }
    });
}

void Input::update(ex::EntityManager& entities,
            ex::EventManager& events,
            ex::TimeDelta dt){
  events.emit(_ial->get_active());
  _ial->clear();
}

}
