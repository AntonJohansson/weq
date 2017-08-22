#include <weq/Application.hpp>
#include <weq/event/Internal.hpp>
#include <weq/event/Window.hpp>
#include <weq/Window.hpp>

#include <chrono>

using namespace std::chrono_literals;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::duration;

namespace weq{

Application::Application(){
  // Initialize Logging context
  _console = spdlog::stdout_color_mt("console");
  spdlog::set_pattern("[%H:%M:%S] %v");

  _events.subscribe<event::Quit>(*this);

  // Create window (TODO Move?)
  _window = std::make_shared<Window>(_events); // Will also initlize glfw/glad
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
