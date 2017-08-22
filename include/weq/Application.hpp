#pragma once

#ifdef _WIN32
#include <Windows.h> // lol wtf... required by spdlog
#endif

#include <weq/resource/ResourceManager.hpp>
#include <weq/state/StateManager.hpp>

#include <entityx/entityx.h>
#include <spdlog/spdlog.h>

// Defined in <weq/Window.hpp>
class Window;
// Defined in <weq/event/Internal.hpp>
namespace event{struct Quit;}

namespace weq{

namespace ex = entityx;
using ex::EventManager;
using ex::EntityManager;
using ex::SystemManager;

// Helper class for interfacing with the engine,
// creates and initializes all necessary managers
// (events, systems, entities, states, resources).
class Application : public ex::Receiver<Application>{
public:
  Application(int argc, char** argv);
  virtual ~Application();

  // Function that should add and configure all systems.
  virtual void configure(){}

  // Temporary function what will open a window.
  void open_window();

  // Responsible for updating systems, called from main_loop().
  virtual void update(double dt){(void)dt;}

  // Fixed timestep update loop that drives the entire simulation,
  // calls update(double dt) which is responsible for dispatching
  // system update calls.
  void run();

  // Handles internal engine events such as Quit
  void receive(const event::Quit& q);

protected:
  bool _should_quit = false; // TODO should be handled by some statemanager?
  EventManager _events;
  EntityManager _entities{_events};
  SystemManager _systems{_entities, _events};

  ResourceManager _resource_manager;
  StateManager _state_manager; // TODO not functioning

  std::shared_ptr<spdlog::logger> _console;
  std::shared_ptr<Window> _window;
};

} // namespace weq

