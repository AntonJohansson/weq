#include <weq/Application.hpp>
#include <weq/event/Internal.hpp>

#include <chrono>

using namespace std::chrono_literals;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::duration;

namespace weq{

Application::Application(){
  _console = spdlog::stdout_color_mt("console");
  spdlog::set_pattern("[%H:%M:%S] %v");
  _events.subscribe<event::Quit>(*this);
}


void Application::run(){
  using Clock = std::chrono::high_resolution_clock;

  //constexpr nanoseconds timestep(16ms);
  constexpr nanoseconds timestep(1ms);
  constexpr double timestep_value = duration_cast<duration<double>>(timestep).count();

  nanoseconds lag{0ns};

  auto start_time = Clock::now();
  auto new_time = start_time;
  nanoseconds delta_time;

  while(!_should_quit){
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
