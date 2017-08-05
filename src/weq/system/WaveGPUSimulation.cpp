#include <weq/system/WaveGPUSimulation.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Wave.hpp>
#include <weq/event/RegisterInput.hpp>

#include <spdlog/spdlog.h>

#include <glad/glad.h> //@TODO required?
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <functional>
#include <deque>
#include <iostream>
#include <cmath>

using component::Renderable;
using component::WaveGPU;

namespace weq::system{

void WaveGPUSimulation::configure(ex::EventManager& events){
  events.subscribe<event::ActiveInput>(*this);
}

void WaveGPUSimulation::update(ex::EntityManager& entities,
                            ex::EventManager& events,
                            ex::TimeDelta dt){
  (void)events;
  (void)dt;

  entities.each<WaveGPU, Renderable>([](ex::Entity e, WaveGPU& wave, Renderable& r){
    });
}

void WaveGPUSimulation::receive(const event::ActiveInput& event){
  (void)event;
}

}
