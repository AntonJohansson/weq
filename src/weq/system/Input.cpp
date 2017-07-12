#include <weq/system/Input.hpp>
#include <weq/Window.hpp>
#include <GLFW/glfw3.h>

#include <iostream>

namespace weq::system{

namespace{
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  }
}

void Input::configure(ex::EventManager& events){
  // set the context to the current one
  // @TODO easy way to switch between contexts
  _window_context = Window::current();

  // Could I loop through all the contexts and force them to this
  // callback?
  glfwSetKeyCallback(_window_context, key_callback);
}

void Input::update(ex::EntityManager& entities,
            ex::EventManager& events,
            ex::TimeDelta dt){
}

}
