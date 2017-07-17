#include <weq/Engine.hpp>
#include <weq/system/Input.hpp>
#include <weq/system/WaveSimulation.hpp>
#include <weq/system/Renderer.hpp>
#include <weq/RunningAverage.hpp>

#include <spdlog/spdlog.h>

#include <chrono>

namespace weq::engine{
namespace{
  namespace ex = entityx;
  ex::EventManager _events;
  ex::EntityManager _entities(_events);
  ex::SystemManager _systems(_entities, _events);
  auto console = spdlog::stderr_color_mt("console");
}

void initialize(){
  _systems.add<system::Input>();
  _systems.add<system::WaveSimulation>();
  _systems.add<system::Renderer>();
  _systems.configure();
}

void main_loop(){
  using namespace std::chrono_literals;
  using Clock = std::chrono::high_resolution_clock;

  constexpr std::chrono::nanoseconds timestep(16ms);
  constexpr double timestep_value = std::chrono::duration_cast<std::chrono::duration<double>>(timestep).count();
  std::chrono::nanoseconds lag(0ns);
  RunningAverage<double> update_time;

  auto start_time = Clock::now();
  while(true){
    auto new_time = Clock::now();
    auto delta_time = new_time - start_time;
    start_time = new_time;

    lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);
    while(lag >= timestep){
      lag -= timestep;

      auto before = Clock::now();
      _systems.update_all(timestep_value);
      auto time = std::chrono::duration_cast<std::chrono::duration<double>>(Clock::now() - before).count();
      update_time.add(time);
      console->info(update_time.average());
    }
    update_time.clear();
  }
}

ex::Entity create_entity(){
  return _entities.create();
}

float width(){return (float)640;}
float height(){return (float)480;}

};
