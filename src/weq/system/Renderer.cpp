#include <weq/system/Renderer.hpp>
#include <weq/Camera.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/Window.hpp>
#include <glad/glad.h>

#include <iostream>
#include <thread>

namespace weq::system{

using component::Renderable;
using component::Transform;

Renderer::Renderer(){
}

Renderer::~Renderer(){
}

void Renderer::configure(ex::EventManager& events){
  _window = std::make_unique<Window>();
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void Renderer::update(ex::EntityManager& entities,
                      ex::EventManager& events,
                      ex::TimeDelta dt){
  camera::update();

  _window->clear(0.0f, 0.0f, 0.0f, 1.0f);

  entities.each<Renderable, Transform>([dt](ex::Entity e,
                                            Renderable& r,
                                            Transform& t){
      r.shader->use();
      r.shader->set("model", t.transform);
      r.shader->set("view", camera::view());
      r.shader->set("projection", camera::projection());
      r.shader->set("normal_matrix", camera::normal_matrix());


      for(const auto& vao : r.mesh->vaos()){
        vao.bind();
        r.mesh->bind_ebo();
        glDrawElements(GL_TRIANGLES, r.mesh->indices(), GL_UNSIGNED_INT, 0);
      }
    });

  _window->swap_buffers();
  Window::poll_events();
}

}
