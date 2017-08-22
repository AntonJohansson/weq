#include <weq/system/WaveGPUSimulation.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Wave.hpp>
#include <weq/event/Input.hpp>
#include <weq/Texture.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>

#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <imgui/imgui.h>

using component::Renderable;
using component::WaveGPU;

// TODO change format of texture buffers.

namespace weq::system{

namespace{
  std::shared_ptr<Mesh> screen_mesh;
  std::shared_ptr<gl::ShaderProgram> vel_shader;
  std::shared_ptr<gl::ShaderProgram> drop_shader;
  std::shared_ptr<gl::ShaderProgram> height_shader;
  bool spawn_drop = false;
}

void WaveGPUSimulation::configure(ex::EventManager& events){
  events.subscribe<event::ActiveInput>(*this);

  // Setup Shaders

  // Height calculation shader

  auto height_v = std::make_shared<gl::Shader>("height.vert");
  auto height_f = std::make_shared<gl::Shader>("height.frag");
  height_v->load();
  height_f->load();
  height_shader = std::make_shared<gl::ShaderProgram>("height.prog",
                                                      height_v,
                                                      height_f);
  height_shader->load();
  height_shader->link();

  // Velocity calculation shader
  auto vel_v = std::make_shared<gl::Shader>("vel.vert");
  auto vel_f = std::make_shared<gl::Shader>("vel.frag");
  vel_v->load();
  vel_f->load();
  vel_shader = std::make_shared<gl::ShaderProgram>("vel.prog",
                                                   vel_v,
                                                   vel_f);
  vel_shader->load();
  vel_shader->link();

  // Drop calculation shader
  auto drop_v = std::make_shared<gl::Shader>("drop.vert");
  auto drop_f = std::make_shared<gl::Shader>("drop.frag");
  drop_v->load();
  drop_f->load();
  drop_shader = std::make_shared<gl::ShaderProgram>("drop.prog",
                                                    drop_v,
                                                    drop_f);
  drop_shader->load();
  drop_shader->link();


  // Setup 2D mesh for rendering textures
  gl::VertexFormat VT2 = {{
      {"position", gl::Type::FLOAT, 2},
      {"texcoord", gl::Type::FLOAT, 2}
    }};

  MeshData screen_mesh_data(VT2);
  screen_mesh_data.interleaved = {
    -1.0f,  1.0f,  0.0f, 1.0f, // top left
    1.0f,  1.0f,  1.0f, 1.0f, // top right
    1.0f, -1.0f,  1.0f, 0.0f, // bottom right
    -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
  };

  screen_mesh_data.elements = {
    0, 1, 2,
    2, 3, 0
  };

  screen_mesh = std::make_shared<Mesh>(screen_mesh_data,
                                       gl::DrawMode::TRIANGLES);
  screen_mesh->generate_vao(height_shader);
  screen_mesh->generate_vao(vel_shader);
  screen_mesh->generate_vao(drop_shader);
}

void WaveGPUSimulation::update(ex::EntityManager& entities,
                            ex::EventManager& events,
                            ex::TimeDelta dt){
  (void)events;
  (void)dt;

  entities.each<WaveGPU, Renderable>([dt](ex::Entity e, WaveGPU& wave, Renderable& r){
      (void)e;

      // Retrive old viewport settings.
      GLint old_viewport[4];
      glGetIntegerv(GL_VIEWPORT, old_viewport);
      glViewport(0, 0, wave.width, wave.height);

      // Bind height fbo
      wave.height_fbo.bind();
      wave.height_fbo.texture()->bind(0);

      // Drop shader
      if(spawn_drop){
        drop_shader->use();
        drop_shader->set("height_field", 0);

        screen_mesh->vao(drop_shader->id()).bind();
        screen_mesh->ebo().bind();

        glDrawElements(GLenum(screen_mesh->draw_mode()),
                       screen_mesh->ebo().size(), GL_UNSIGNED_INT, 0);

        spawn_drop = false;
      }

      wave.height_fbo.bind();
      ImGui::Begin("Debug");
      ImGui::Text("Input to the velocity shader:");
      ImGui::Image((void*)wave.height_fbo.texture()->handle(), ImVec2(200, 200));
      ImGui::End();

      // Velocity shader
      wave.vel_fbo.bind();

      wave.height_fbo.texture()->bind(0);
      wave.vel_fbo.texture()->bind(1);

      vel_shader->use();
      vel_shader->set("height_field", 0);
      vel_shader->set("vel_field", 1);
      vel_shader->set("dt", dt);
      vel_shader->set("gridsize", glm::vec2(1.0/wave.width, 1.0/wave.height));
      vel_shader->set("r", wave.r);

      screen_mesh->vao(vel_shader->id()).bind();
      screen_mesh->ebo().bind();

      glDrawElements(GLenum(screen_mesh->draw_mode()),
                     screen_mesh->ebo().size(), GL_UNSIGNED_INT, 0);

      wave.vel_fbo.unbind();

      // Height update shader

      wave.height_fbo.bind();

      wave.height_fbo.texture()->bind(0);
      wave.vel_fbo.texture()->bind(1);

      height_shader->use();
      height_shader->set("height_field", 0);
      height_shader->set("vel_field", 1);
      height_shader->set("dt", dt);

      screen_mesh->vao(height_shader->id()).bind();
      screen_mesh->ebo().bind();

      glDrawElements(GLenum(screen_mesh->draw_mode()),
                     screen_mesh->ebo().size(), GL_UNSIGNED_INT, 0);

      wave.height_fbo.unbind();

      // Reset viewport settings.
      glViewport(old_viewport[0],
                 old_viewport[1],
                 old_viewport[2],
                 old_viewport[3]);

      r.texture = wave.height_fbo.texture();

      // Visualize the output from each shader.
      ImGui::Begin("Debug");
        ImGui::Text("Output from velocity shader:");
        ImGui::Image((void*)wave.vel_fbo.texture()->handle(), ImVec2(200, 200));

        ImGui::Text("Output from height shader:");
        ImGui::Image((void*)wave.height_fbo.texture()->handle(), ImVec2(200, 200));
      ImGui::End();
    });
}

void WaveGPUSimulation::receive(const event::ActiveInput& event){
  (void)event;
  if(event.has(InputAction::SPAWN_WAVELET)){
    spawn_drop = true;
  }
}

}
