#include <weq/Application.hpp>
#include <weq/event/Internal.hpp>
#include <weq/event/Window.hpp>
#include <weq/Window.hpp>

#include <weq/RunningAverage.hpp>

#include <weq/memory/ResourceManager.hpp>

#include <string>

#include <iostream>
#include <iomanip>


// YO SNYGGING
// YO SNYGGING
// YO SNYGGING
// YO SNYGGING
// get width/height/mode from runtime options (ezpz).

// Temporary anonymous namspace, used to store window info
// inferred from cmd args.
namespace{
  unsigned int width = 1280, height = 720;
  WindowMode mode = WindowMode::WINDOWED;
}

namespace weq{

Application::Application(int argc, char** argv){

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
  memory::resource_manager::initialize(_events);

  // Subscribe to internal engine events
  _events.subscribe<event::Quit>(*this);

  // Create window (TODO Move!!)
  _window = std::make_shared<Window>(_events, "Wave Simulation", width, height, mode); // Will also initlize glfw/glad
}

Application::~Application(){
  memory::resource_manager::shutdown();
}

void Application::run(){
  // TODO move
  _events.emit(event::ActiveWindow(_window)); // This event will not buffer.

  using Clock = std::chrono::high_resolution_clock;

  auto start_time = Clock::now();
  auto new_time = start_time;
  nanoseconds delta_time;

  RunningAverage<double> frame_time;
  nanoseconds accum{0s};
  float frames = 0.0f;

  while(!_should_quit){
    // Handle window quit events.
    if(_window->should_close())_should_quit = true;

    // Update delta t.
    new_time = Clock::now();
    delta_time = new_time - start_time;
    start_time = new_time;

    _lag += delta_time;
    accum += delta_time;

    // Update lag time for all systems
    _systems.for_each([&delta_time](auto system){
        system->set_lag(system->get_lag() + delta_time);
      });

    if(accum >= 1s){
      frame_time.add(frames);
      _current_update_frequency = frame_time.average();

      _systems.for_each([](auto system){
          system->second_has_past();
        });

      frames = 0.0f;
      accum = 0s;
    }

    while(_lag >= _timestep){
      frames++;
      _systems.for_each([&delta_time, this](auto system){
          if(system->get_timestep() < _timestep){
            _timestep = system->get_timestep();
            _timestep_value = system->get_timestep_value();
          }

          if(system->get_lag() >= system->get_timestep()){
            system->update(_entities, _events, system->get_timestep_value());
            system->set_lag(system->get_lag() - system->get_timestep());
            system->increment_frame_counter();
          }
        });

      _lag -= _timestep;
    }
  }

}

void Application::receive(const event::Quit& q){
  (void)q;
  _should_quit = true;
}

} // namespace weq
