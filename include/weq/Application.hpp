#pragma once

#ifdef _WIN32
#include <Windows.h> // lol wtf... required by spdlog
#undef near
#undef far
#endif

#include <weq/memory/ResourceManager.hpp>
#include <weq/state/StateManager.hpp>

#include <weq/ecs/Fwd.hpp>
#include <weq/ecs/Receiver.hpp>
//#include <entityx/entityx.h>
#include <spdlog/spdlog.h>

#include <string>

#include <chrono>
using namespace std::chrono_literals;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::duration;




namespace weq{

// Defined in <weq/Window.hpp>
class Window;

// Defined in <weq/event/Internal.hpp>
namespace event{struct Quit;}

//namespace ex = entityx;
//using ex::EventManager;
//using ex::EntityManager;
//using ex::SystemManager;

// Helper class for interfacing with the engine,
// creates and initializes all necessary managers
// (events, systems, entities, states, resources).
//class Application : public Receiver<Application>{
class Application : public Receiver{
public:
  Application(int argc, char** argv);
  virtual ~Application();

  // Function that should add and configure all systems.
  virtual void configure(){}

  // Responsible for updating systems, called from main_loop().
  virtual void update(double dt){(void)dt;}

  // Fixed timestep update loop that drives the entire simulation,
  // calls update(double dt) which is responsible for dispatching
  // system update calls.
  void run();

  // Handles internal engine events such as Quit
  void receive(event::Quit& q);

protected:
  bool _should_quit = false; // TODO should be handled by some statemanager?

  double _current_update_frequency = 0.0;
  nanoseconds _timestep{16ms};
  nanoseconds _lag{0ns};
  double _timestep_value{0.016};

  std::shared_ptr<EventManager>  _events;
  std::shared_ptr<EntityManager> _entities;
  std::shared_ptr<SystemManager> _systems;
  //EventManager _events;
  //EntityManager _entities{_events};
  //SystemManager _systems{_entities, _events};

  //ResourceManager _resource_manager;
  state::StateManager _state_manager; // TODO not functioning

  std::shared_ptr<spdlog::logger> _console;
  std::shared_ptr<Window> _window;
};

} // namespace weq

