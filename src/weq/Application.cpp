#include <weq/Application.hpp>
#include <weq/event/Internal.hpp>
#include <weq/event/Window.hpp>
#include <weq/Window.hpp>

#include <string>
#include <chrono>

using namespace std::chrono_literals;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::duration;

// YO SNYGGING
// YO SNYGGING
// YO SNYGGING
// YO SNYGGING
// get width/height/mode from runtime options (ezpz).

namespace weq{

Application::Application(int argc, char** argv){
  int width, height;
  WindowMode mode;

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

  // Create window (TODO Move?)
  _window = std::make_shared<Window>(_events, "Wave Simulation", width, height, mode); // Will also initlize glfw/glad
}

Application::~Application(){
}

void Application::run(){
  // TODO move
  _events.emit(event::ActiveWindow(_window)); // This event will not buffer.

  using Clock = std::chrono::high_resolution_clock;

  constexpr nanoseconds timestep(16ms);
  //constexpr nanoseconds timestep(1ms);
  constexpr double timestep_value = duration_cast<duration<double>>(timestep).count();

  nanoseconds lag{0ns};

  auto start_time = Clock::now();
  auto new_time = start_time;
  nanoseconds delta_time;

  while(!_should_quit){
    // Handle window quit events.
    if(_window->should_close())_should_quit = true;

    // Update delta t.
    new_time = Clock::now();
    delta_time = new_time - start_time;
    start_time = new_time;

    lag += delta_time;

    while(lag >= timestep){
      lag -= timestep;

      update(timestep_value);
    }
  }

}

void Application::receive(const event::Quit& q){
  (void)q;
  _should_quit = true;
}

}
