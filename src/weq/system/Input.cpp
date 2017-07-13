#include <weq/system/Input.hpp>
#include <weq/system/InputAbstractionLayer.hpp>
#include <weq/Window.hpp>

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

  _ial = new InputAbstractionLayer({
      {},
      {
        {GLFW_KEY_W, InputState::MOVE_FORWARD},
        {GLFW_KEY_A, InputState::MOVE_LEFT},
        {GLFW_KEY_S, InputState::MOVE_BACK},
        {GLFW_KEY_D, InputState::MOVE_RIGHT},
        {GLFW_KEY_LEFT_SHIFT, InputState::MOVE_UP},
        {GLFW_KEY_LEFT_CONTROL, InputState::MOVE_DOWN}
        },
      {}
    });
}

void Input::update(ex::EntityManager& entities,
            ex::EventManager& events,
            ex::TimeDelta dt){
  events.emit(_ial->get_active());
  _ial->clear();
}

}
