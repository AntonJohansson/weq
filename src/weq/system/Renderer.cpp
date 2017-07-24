#include <weq/system/Renderer.hpp>
#include <weq/Camera.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Camera.hpp>
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

  //camera::update();

  _window->clear(0.0f, 0.0f, 0.0f, 1.0f);

  component::Camera active_camera;
  entities.each<component::Camera, component::ActiveCamera>([&active_camera](ex::Entity e, component::Camera& c, component::ActiveCamera& a){active_camera = c;});

  entities.each<Renderable, Transform>([dt, &active_camera](ex::Entity e,
                                                            Renderable& r,
                                                            Transform& t){
      r.program->use();
      r.program->set("model", t.transform);
      r.program->set("view", active_camera.view);
      r.program->set("projection", active_camera.projection);
      r.program->set("normal_matrix", active_camera.normal_matrix);

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
  }

}

}
