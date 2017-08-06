#include <weq/system/WaveGPUSimulation.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Wave.hpp>
#include <weq/event/Input.hpp>

#include <spdlog/spdlog.h>

#include <glad/glad.h>

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
