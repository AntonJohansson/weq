#include <weq/Engine.hpp>
#include <weq/Camera.hpp>
#include <weq/system/Input.hpp>
#include <weq/system/Renderer.hpp>

#include <thread>
#include <iostream>
#include <chrono>

namespace weq::engine{
namespace{
  namespace ex = entityx;
  ex::EventManager _events;
  ex::EntityManager _entities(_events);
  ex::SystemManager _systems(_entities, _events);
}

void initialize(){
  _systems.add<system::Input>();
  _systems.add<system::Renderer>();
  _systems.configure();

  camera::set_aspect(640.0/480.0f);
  camera::set_fov(45.0f);
  camera::set_pos({0, 0, 10});
  camera::set_dir({0, 0, -1});
  camera::calculate_perspective();
}

void main_loop(){
  using namespace std::chrono_literals;
  using Clock = std::chrono::high_resolution_clock;

  constexpr std::chrono::nanoseconds timestep(16ms);
  std::chrono::nanoseconds lag(0ns);

  auto start_time = Clock::now();
  while(true){
    auto new_time = Clock::now();
    auto delta_time = new_time - start_time;
    start_time = new_time;

    lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);
    while(lag >= timestep){
      lag -= timestep;

      _systems.update_all(std::chrono::duration_cast<std::chrono::duration<float>>(timestep).count());
    }
  }
}

ex::Entity create_entity(){
  return _entities.create();
}

float width(){return (float)640;}
float height(){return (float)480;}

};
