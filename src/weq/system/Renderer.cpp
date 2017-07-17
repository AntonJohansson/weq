#include <weq/system/Renderer.hpp>
#include <weq/Camera.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/Window.hpp>
#include <weq/event/RegisterInput.hpp>
#include <glad/glad.h>


#include <glm/gtc/matrix_transform.hpp>

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

  events.subscribe<event::ActiveInput>(*this);

  camera::set_aspect(640.0/480.0f);
  camera::set_fov(45.0f);
  camera::set_pos({0, 0, 10});
  camera::set_dir({0, 0, -1});
  camera::calculate_perspective();
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
      r.shader->set("normal_matrix", glm::transpose(glm::inverse(camera::view()*t.transform)));

      r.mesh->vao().bind();
      r.mesh->bind_ebo();
      glDrawElements(GL_TRIANGLES, r.mesh->indices(), GL_UNSIGNED_INT, 0);
    });

  _window->swap_buffers();
  Window::poll_events();
}

void Renderer::receive(const event::ActiveInput &event){
  float speed = 0.05f;
  if(event.has(InputRange::CURSOR_X) && event.has(InputRange::CURSOR_Y)){
    static float last_x = 0.0f;
    static float last_y = 0.0f;

    const float& x = event.ranges.at(InputRange::CURSOR_X);
    const float& y = event.ranges.at(InputRange::CURSOR_Y);

    auto dx = x - last_x;
    auto dy = y - last_y;

    auto up = camera::up();
    auto dir = camera::direction();
    auto right = glm::cross(dir, up);
    auto local_up = glm::cross(right, dir);

    camera::rotate_deg(10.0f*dx, local_up);
    camera::rotate_deg(10.0f*dy, right);

    last_x = x;
    last_y = y;
  }

  if(event.has(InputState::MOVE_LEFT)){
    camera::move_right(-speed);
  }if(event.has(InputState::MOVE_RIGHT)){
    camera::move_right(speed);
  }if(event.has(InputState::MOVE_FORWARD)){
    camera::move_forward(speed);
  }if(event.has(InputState::MOVE_BACK)){
    camera::move_forward(-speed);
  }if(event.has(InputState::MOVE_UP)){
    camera::move_up(speed);
  }if(event.has(InputState::MOVE_DOWN)){
    camera::move_up(-speed);
  }
}

}
