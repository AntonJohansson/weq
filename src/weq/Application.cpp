#include <weq/Application.hpp>
#include <weq/event/Internal.hpp>
#include <weq/event/Window.hpp>
#include <weq/Window.hpp>

#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>
#include <weq/ecs/SystemManager.hpp>

#include <weq/RunningAverage.hpp>

#include <weq/memory/ResourceManager.hpp>

#include <weq/gl/ErrorHandler.hpp>

//#include <string>
//
//#include <iostream>
//#include <iomanip>

namespace weq{

// Temporary anonymous namspace, used to store window info
// inferred from cmd args.
namespace{
  unsigned int width = 1280, height = 720;
  WindowMode mode = WindowMode::WINDOWED;
}

Application::Application(int argc, char** argv){
  // Initialize managers
  _events   = std::make_shared<EventManager>();
  _entities = std::make_shared<EntityManager>();
  _systems  = std::make_shared<SystemManager>();

  // Initialize Logging context
  _console = spdlog::stdout_color_mt("console");
  spdlog::set_pattern("[%H:%M:%S] %v");

  // Parse command-line arguments (i = 1 -> skip executable path argument)
  for(int i = 1; i < argc; i++){
    std::string str(argv[i]);

    if(str == "-w"){
      width = std::stoi(argv[++i]);
    }else if(str == "-h"){
      height = std::stoi(argv[++i]);
    }else if(str == "-m"){
      std::string mode_str(argv[++i]);
      if(mode_str == "window"){
        mode = WindowMode::WINDOWED;
      }else if(mode_str == "fullscreen"){
        mode = WindowMode::FULLSCREEN;
      }else if(mode_str == "borderless"){
        mode = WindowMode::WINDOWED_FULLSCREEN;
      }else{
        _console->warn("Unrecognized window mode \"{}\"", mode_str);
      }
    }else{
      _console->warn("Unrecognized flag \"{}\"", str);
    }
  }

  // Initialize resource manager
  memory::resource_manager::initialize(*_events);

  // Subscribe to internal engine events
  _events->subscribe<event::Quit>(*this);

  // Create window (TODO Move!!)
  _window = std::make_shared<Window>(*_events, "Wave Simulation", width, height, mode); // Will also initlize glfw/glad
  setup_gl_error_callback();
}

Application::~Application(){
  memory::resource_manager::shutdown();
}

void Application::run(){
  // TODO move
  _events->emit(event::ActiveWindow(_window)); // This event will not buffer.

  using Clock = std::chrono::high_resolution_clock;

  auto start_time = Clock::now();
  auto new_time = start_time;
  nanoseconds delta_time{ 0ns};

  nanoseconds accum{0s};


  /*
    TRY THIS

    - update all systems with the same freqency 1000 hz
    - see if the same problem remains
    - do somethings else cuz this didn't work :)

   */

  //_timestep = 1000000ns;
  //_timestep_value = 0.001;
  //_timestep = 16ms;
  //_timestep_value = 0.016;

  // Here's an idea:
  // - have an effective fps: factor * max_fps
  //   if we cannot reach the target max_fps
  //   and change this factor depending on, hmmmm
  // - test having a system have a target fps, slightly above what is capable
  //   see how it reacts, if it's fine, the above approach might work.

  while (!_should_quit) {
	  // Handle window quit events.
	  if (_window->should_close())_should_quit = true;

	  // Update delta t.
	  new_time = Clock::now();
	  delta_time = new_time - start_time;
	  start_time = new_time;

	  accum += delta_time;

	  // Update lag time for all systems
	  _systems->for_each([&delta_time](auto system) {
		  system->set_lag(system->get_lag() + delta_time);
	  });

	  // Update fps counters for all systems
	  // @@@@@@@@@@@NOTE
	  // - if we take longer than one s (ex. 2 s) to update (might be stuck in a lag loop)
	  // - divide frame number by that then :)
	  if (accum >= 1s) {
		  int seconds = (int)std::chrono::duration_cast<std::chrono::seconds>(accum).count();

		  _systems->for_each([&seconds](auto system) {
			  system->second_has_past(seconds);
			  //if (system->get_framerate() < 0.85*system->get_desired_framerate()) {
        //  system->set_factor(1.2*system->get_factor());
				//  spdlog::get("console")->info("LAG {}", system->get_factor());
			  //}
		  });

		  accum = 0s;
	  }

	  // Update systems
	  // - when trying to run at a high fps (ex. ~1000)
	  // - app gets stuck in this loop
	  // Added extra check for _should_quit, since if the engine can't keep up,
	  // it will get stuck in the lag loop, thus never checking.
	  int number_of_systems = _systems->size();
	  //while(_lag >= _timestep && !_should_quit){
	  while (number_of_systems > 0 && !_should_quit) {
		  _systems->for_each([&delta_time, &number_of_systems, this](auto system) {
			  // Update stuff
			  if (system->get_lag() >= system->get_timestep()) {
				  //spdlog::get("console")->info("do update");
				  system->update(*_entities, *_events, system->get_timestep_value());
				  //system->update(*_entities, *_events, _timestep_value);
				  system->update_lag();
				  //system->set_lag(system->get_lag() - system->get_timestep());
				  system->increment_frame_counter();

			  }
			  else if (system->active()) {
				  number_of_systems--;
				  system->set_active(false);
			  }
		  });
	  }

	  // Reactivate all systems
	  _systems->for_each([](auto s) {s->set_active(true);});
  }
}

void Application::receive(event::Quit& q){
  (void)q;
  _should_quit = true;
}

} // namespace weq
