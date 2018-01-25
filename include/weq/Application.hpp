#pragma once

#ifdef _WIN32
#include <Windows.h> // lol wtf... required by spdlog
#undef near
#undef far
#endif

#include <weq/memory/ResourceManager.hpp>
#include <weq/state/StateManager.hpp>

#include <entityx/entityx.h>
#include <spdlog/spdlog.h>

#include <chrono>
using namespace std::chrono_literals;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::duration;



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

  // Responsible for updating systems, called from main_loop().
  virtual void update(double dt){(void)dt;}

  void update_systems();


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

