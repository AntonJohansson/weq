#include <weq/system/Renderer.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Camera.hpp>
#include <weq/Window.hpp>
#include <weq/event/RegisterInput.hpp>

#include <weq/gl/VertexFormat.hpp>
#include <weq/gl/Framebuffer.hpp>
#include <weq/Texture.hpp>
#include <weq/resource/ResourceManager.hpp>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <spdlog/spdlog.h>

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
  //glEnable(GL_SCISSOR_TEST);

  events.subscribe<event::ActiveInput>(*this);

  texture = std::make_shared<Texture>("cloudtop_bk.tga");
  texture->load();
}

void Renderer::update(ex::EntityManager& entities,
                      ex::EventManager& events,
                      ex::TimeDelta dt){
  (void)events;

  static glm::mat4 mvp;

  //_window->clear(0.0f, 0.0f, 0.0f, 1.0f);

  component::Camera active_camera;
  entities.each<component::Camera, component::ActiveCamera>([&active_camera](ex::Entity e, component::Camera& c, component::ActiveCamera& a){active_camera = c;});

  // caluclate vp-matrix
  active_camera.viewproj = active_camera.projection * active_camera.view;

  // Move draw code out of entities loop, works fine since there's only a
  // single entity.
  entities.each<Renderable, Transform>([dt, &active_camera](ex::Entity e,
                                                            Renderable& r,
                                                            Transform& t){
      // Draw scene to fbo

      r.fbo.bind();

      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);

      // calculate mvp for each model
      mvp = active_camera.viewproj * t.model();
      r.scene->use();
      r.scene->set("mvp", mvp);
      r.scene->set("normal_matrix", active_camera.normal_matrix);

      r.mesh->vao(r.scene->id()).bind();
      r.mesh->ebo().bind();

      glDrawElements(GLenum(r.draw_mode), r.mesh->ebo().size(), GL_UNSIGNED_INT, 0);

      glDisable(GL_DEPTH_TEST);
      r.fbo.unbind();

      // Draw screen from fbo

      r.screen->use();
      r.screen->set("framebuffer", 0);

      r.screen_mesh->vao(r.screen->id()).bind();
      r.screen_mesh->ebo().bind();

      r.fbo.texture()->bind(0);

      glDrawElements(GLenum(r.draw_mode), r.screen_mesh->ebo().size(), GL_UNSIGNED_INT, 0);
    });

  render_ui(entities, events, dt);

  _window->swap_buffers();

  // Check for OpenGL errors (TODO should be able to disable this?)
  GLenum err;
  while((err = glGetError()) != GL_NO_ERROR){
    spdlog::get("console")->error("GL-error: {}", err);
  }
}

void Renderer::render_ui(ex::EntityManager& entities,
                         ex::EventManager& events,
                         ex::TimeDelta dt){
  ImGui_ImplGlfwGL3_NewFrame();

  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(20/255.f, 20/255.f, 20/255.f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(40/255.f, 40/255.f, 40/255.f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(135/255.f, 135/255.f, 135/255.f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(20/255.f, 20/255.f, 20/255.f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(40/255.f, 40/255.f, 40/255.f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(135/255.f, 135/255.f, 135/255.f, 1.0f));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

  entities.each<component::ImGui>([dt, &events](ex::Entity e, component::ImGui& i){
      if(i._register_ui)i._register_ui(events);
    });

  ImGui::PopStyleVar();
  ImGui::PopStyleColor(6);

  ImGui::Render();
}

void Renderer::receive(const event::ActiveInput &event){
  if(event.has(InputState::CURSOR_DOWN)){
    _window->set_cursor_mode(CursorMode::DISABLED);
  }else{
    // TODO should probably sent out a "state-changed" event
    _window->set_cursor_mode(CursorMode::NORMAL);
  }

  if(event.has(InputAction::RENDER_WIREFRAME)){
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }else if(event.has(InputAction::RENDER_SOLID)){
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }

}

}
