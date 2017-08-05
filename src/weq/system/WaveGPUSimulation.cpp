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
      (void)e;

      wave.force_fbo.bind();

      // remove this clear
      glClearColor(0,0,0,1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      wave.force_shader->use();

      glViewport(0, 0, wave.width, wave.height);

      r.mesh->vao(wave.force_shader->id()).bind();
      r.mesh->ebo().bind();

      glDrawElements(GLenum(r.draw_mode), r.mesh->ebo().size(), GL_UNSIGNED_BYTE, 0);

      //wave.force_fbo.unbind();
      glViewport(0, 0, 1280, 720);

      glActiveTexture(GL_TEXTURE0);
      wave.force_fbo.bind_texture();

      unsigned char pixels[4];
      glReadPixels(0,0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
      //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
      spdlog::get("console")->info("{}, {}, {}", pixels[0], pixels[1], pixels[2]);

      wave.force_fbo.unbind();
    });
}

void WaveGPUSimulation::receive(const event::ActiveInput& event){
  (void)event;
}

}
