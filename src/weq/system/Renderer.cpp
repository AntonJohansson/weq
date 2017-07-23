#include <weq/system/Renderer.hpp>
#include <weq/Camera.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/Window.hpp>
#include <weq/event/RegisterInput.hpp>

#include <weq/Engine.hpp> // @TEMP
#include <weq/Texture.hpp>
#include <weq/resource/ResourceManager.hpp>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include <thread>

namespace weq::system{
std::shared_ptr<Texture> texture;

using component::Renderable;
using component::Transform;

Renderer::Renderer(){
}

Renderer::~Renderer(){
}

void Renderer::configure(ex::EventManager& events){
  _window = std::make_unique<Window>(events);

  //glEnable(GL_BLEND);
  //glBlendEquation(GL_FUNC_ADD);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_SCISSOR_TEST);

  events.subscribe<event::ActiveInput>(*this);

  //camera::set_aspect(640.0/480.0f);
  //camera::set_fov(45.0f);
  //camera::set_pos({0, 0, 10});
  //camera::set_dir({0, 0, -1});
  //camera::calculate_perspective();

  //texture = engine::resource_mgr()->get<Texture>("sample.png");
  //texture = engine::resource_mgr()->get<Texture>("cloudtop_bk.tga");
  //texture->bind(); //TODO TEMP
}

void Renderer::update(ex::EntityManager& entities,
                      ex::EventManager& events,
                      ex::TimeDelta dt){
  (void)events;

  camera::update();

  _window->clear(0.0f, 0.0f, 0.0f, 1.0f);

  entities.each<Renderable, Transform>([dt](ex::Entity e,
                                            Renderable& r,
                                            Transform& t){
      r.program->use();
      r.program->set("model", t.transform);
      r.program->set("view", camera::view());
      r.program->set("projection", camera::projection());
      r.program->set("normal_matrix", glm::transpose(glm::inverse(camera::view()*t.transform)));

      r.mesh->vao().bind();
      r.mesh->bind_ebo();
      glDrawElements(GL_TRIANGLES, r.mesh->indices(), GL_UNSIGNED_INT, 0);
    });

  _window->swap_buffers();
  Window::poll_events();
}

void Renderer::render_ui(ex::EntityManager& entities,
                         ex::EventManager& events,
                         ex::TimeDelta dt){
  ImGui_ImplGlfwGL3_NewFrame();

  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(20/255.f, 20/255.f, 20/255.f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(40/255.f, 40/255.f, 40/255.f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(135/255.f, 135/255.f, 135/255.f, 1.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

  entities.each<component::ImGui>([dt, &events](ex::Entity e, component::ImGui& i){
      if(i._register_ui)i._register_ui(events);
    });

  ImGui::PopStyleVar();
  ImGui::PopStyleColor(3);

  ImGui::Render();
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
  }if(event.has(InputAction::RENDER_WIREFRAME)){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }if(event.has(InputAction::RENDER_SOLID)){
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

}
