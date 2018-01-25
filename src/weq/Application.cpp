#include <weq/Application.hpp>
#include <weq/event/Internal.hpp>
#include <weq/event/Window.hpp>
#include <weq/Window.hpp>

#include <weq/RunningAverage.hpp>

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

  // Subscribe to internal engine events
  _events.subscribe<event::Quit>(*this);

  // Create window (TODO Move!!)
  _window = std::make_shared<Window>(_events, "Wave Simulation", width, height, mode); // Will also initlize glfw/glad
}

Application::~Application(){
}

void Application::run(){
  // TODO move
  _events.emit(event::ActiveWindow(_window)); // This event will not buffer.

  using Clock = std::chrono::high_resolution_clock;

  nanoseconds timestep{16ms};
  nanoseconds lag{0ns};
  double timestep_value{0.016};

  //constexpr nanoseconds timestep(16ms);
  //constexpr nanoseconds timestep(5ms);
  //constexpr nanoseconds timestep(1ms);
  //constexpr double timestep_value = duration_cast<duration<double>>(timestep).count();

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

    lag += delta_time;
    accum += delta_time;

    _systems.for_each([&lag, &delta_time](auto system){
        system->set_lag(system->get_lag() + delta_time);
      });

    //std::cout << "\r";
    if(frame_time.count() >= 120)frame_time.clear();
    if(accum >= 1s){
      frame_time.add(frames);
      //std::cout << std::setw(10) << frame_time.average() << "/" << 1.0/timestep_value;

      _systems.for_each([](auto system){
          //std::cout << std::setw(10) << system->get_frame_counter() << "/" << 1.0/system->get_timestep_value();
          system->clear_frame_counter();
        });

      frames = 0.0f;
      accum = 0s;
    }
    //std::cout << std::flush;

    while(lag >= timestep){
      frames++;
      _systems.for_each([&timestep, &timestep_value, &lag, &delta_time, this](auto system){
          if(system->get_timestep() < timestep){
            timestep = system->get_timestep();
            timestep_value = system->get_timestep_value();
          }

          if(system->get_lag() >= system->get_timestep()){
            system->update(_entities, _events, system->get_timestep_value());
            system->set_lag(system->get_lag() - system->get_timestep());
            system->increment_frame_counter();
          }
        });

      lag -= timestep;
    }
  }

}

void Application::update_systems(){
}

void Application::receive(const event::Quit& q){
  (void)q;
  _should_quit = true;
}

}
