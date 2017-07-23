#pragma once

#include <weq/resource/ResourceManager.hpp>

#include <entityx/entityx.h>
#include <spdlog/spdlog.h>

namespace event{struct Quit;}

namespace weq{

namespace ex = entityx;
using ex::EventManager;
using ex::EntityManager;
using ex::SystemManager;

class Application : public ex::Receiver<Application>{
public:
  Application();
  virtual ~Application(){}

  virtual void configure(){}
  virtual void update(double dt){(void)dt;}

  void main_loop();

  void receive(const event::Quit& q);

protected:
  bool _should_quit = false;
  EventManager _events;
  EntityManager _entities{_events};
  SystemManager _systems{_entities, _events};
  ResourceManager _resources;
  std::shared_ptr<spdlog::logger> _console;
};

}

