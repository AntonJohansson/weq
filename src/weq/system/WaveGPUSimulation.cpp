#include <weq/system/WaveGPUSimulation.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Wave.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/event/Input.hpp>
#include <weq/Texture.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/primitive/Plane.hpp>

#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <imgui/imgui.h>

#include <future>

using component::Renderable;
using component::WaveGPU;

namespace{
  bool clear = false;
  bool set_c = false;
  bool recompute_mesh = true; // Generate a mesh with default resolution and mesh_size.
  bool refractive_visible = false;
  int wall_item = 0; // 0 - none, 1 - single, 2, - double, 3 - custom
  int boundary_item = 0; // 0 - reflect, 1 - radiate

  int resolution = 1000;
  float mesh_size = 5.0f;
  float c = 0.2f;

  void apply_shader(std::shared_ptr<Mesh> mesh, gl::Framebuffer& fbo, std::shared_ptr<gl::ShaderProgram> shader){
    fbo.bind();

    mesh->vao(shader).bind();
    mesh->ebo().bind();

    glDrawElements(GLenum(mesh->draw_mode()),
                   mesh->ebo().size(), GL_UNSIGNED_INT, 0);
  }
}

namespace weq::system{

namespace{
  std::shared_ptr<Mesh> screen_mesh;
  std::shared_ptr<gl::ShaderProgram> drop_shader;
  std::shared_ptr<gl::ShaderProgram> edge_shader;
  std::shared_ptr<gl::ShaderProgram> clear_shader;
  std::shared_ptr<gl::ShaderProgram> vel_shader;
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

  // Clear shader
  auto clear_v = std::make_shared<gl::Shader>("clear.vert");
  auto clear_f = std::make_shared<gl::Shader>("clear.frag");
  clear_v->load();
  clear_f->load();
  clear_shader = std::make_shared<gl::ShaderProgram>("clear.prog",
                                                     clear_v,
                                                     clear_f);
  clear_shader->load();
  clear_shader->link();

  // Edge shader
  auto edge_v = std::make_shared<gl::Shader>("edge.vert");
  auto edge_f = std::make_shared<gl::Shader>("edge.frag");
  edge_v->load();
  edge_f->load();
  edge_shader = std::make_shared<gl::ShaderProgram>("edge.prog",
                                                    edge_v,
                                                    edge_f);
  edge_shader->load();
  edge_shader->link();

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
  screen_mesh->generate_vao(clear_shader);
  screen_mesh->generate_vao(edge_shader);
}

void WaveGPUSimulation::update(ex::EntityManager& entities,
                            ex::EventManager& events,
                            ex::TimeDelta dt){
  (void)events;
  (void)dt;

  add_ui(entities);

  entities.each<WaveGPU, Renderable>([dt](ex::Entity e, WaveGPU& wave, Renderable& r){
      (void)e;

      // Recompute grid mesh from new resolution
      if(recompute_mesh){
        wave.width = resolution;
        wave.height = resolution;
        wave.height_fbo.resize(resolution, resolution);
        wave.vel_fbo.resize(resolution, resolution);

        recompute_mesh = false;

        // OpenGL is per thread, so this will be anoying to split out.
        r.mesh->set_data(primitive::plane::solid(resolution, resolution,
                                                 mesh_size/(float)resolution,
                                                 gl::format::VNCT));
      }

      // Retrive old viewport settings.
      GLint old_viewport[4];
      glGetIntegerv(GL_VIEWPORT, old_viewport);
      glViewport(0, 0, wave.width, wave.height);


      // Handle interactivity
      if(set_c){wave.set_c(c); set_c = false;}
      if(clear){
        // Apply clear shader on height_fbo and vel_fbo
        clear_shader->use();
        wave.vel_fbo.texture()->bind();
        apply_shader(screen_mesh, wave.vel_fbo, clear_shader);

        wave.height_fbo.texture()->bind();
        apply_shader(screen_mesh, wave.height_fbo, clear_shader);

        clear = false;
      }

      // Apply edge shader if boundary mode is set to radiate.
      if(boundary_item == 1){ // Radiate
        edge_shader->use();
        edge_shader->set("height_field", 0);
        edge_shader->set("gridsize", glm::vec2(1.0/wave.width, 1.0/wave.height));
        edge_shader->set("c", wave.c);
        edge_shader->set("dt", dt);
        wave.height_fbo.texture()->bind(0);
        apply_shader(screen_mesh, wave.height_fbo, edge_shader);
      }

      // Drop shader
      if(spawn_drop){
        drop_shader->use();
        drop_shader->set("height_field", 0);
        wave.height_fbo.texture()->bind(0);
        apply_shader(screen_mesh, wave.height_fbo, drop_shader);

        spawn_drop = false;
      }

      // UI
      wave.height_fbo.bind();
      ImGui::Begin("Debug");
      ImGui::Text("Input to the velocity shader:");
      ImGui::Image((void*)wave.height_fbo.texture()->handle(), ImVec2(200, 200));
      ImGui::End();

      // Velocity shader
      vel_shader->use();
      vel_shader->set("height_field", 0);
      vel_shader->set("vel_field", 1);
      vel_shader->set("dt", dt);
      vel_shader->set("gridsize", glm::vec2(1.0/wave.width, 1.0/wave.height));
      vel_shader->set("r", wave.r);

      wave.height_fbo.texture()->bind(0);
      wave.vel_fbo.texture()->bind(1);

      apply_shader(screen_mesh, wave.vel_fbo, vel_shader);

      // Height update shader
      height_shader->use();
      height_shader->set("height_field", 0);
      height_shader->set("vel_field", 1);
      height_shader->set("dt", dt);
      wave.height_fbo.texture()->bind(0);
      wave.vel_fbo.texture()->bind(1);

      apply_shader(screen_mesh, wave.height_fbo, height_shader);

      // Reset viewport settings.
      glViewport(old_viewport[0],
                 old_viewport[1],
                 old_viewport[2],
                 old_viewport[3]);

      r.textures.clear();
      r.textures.push_back(wave.height_fbo.texture());

      // Visualize the output from each shader.
      ImGui::Begin("Debug");
        ImGui::Text("Output from velocity shader:");
        ImGui::Image((void*)wave.vel_fbo.texture()->handle(), ImVec2(200, 200));

        ImGui::Text("Output from height shader:");
        ImGui::Image((void*)wave.height_fbo.texture()->handle(), ImVec2(200, 200));
      ImGui::End();
    });
}

void WaveGPUSimulation::add_ui(ex::EntityManager& entities){

  if(!_ui_created){
    _ui_created = true;
    _ui = entities.create();
    _ui.assign<component::ImGui>([&](ex::EventManager& e){
        ImGui::Begin("Menu");

        if(ImGui::CollapsingHeader("simulation")){
          ImGui::Separator();
          if(ImGui::InputFloat("Wave velocity (c)", &c)){
            set_c = true;
          }

          if(ImGui::Button("Clear waves")){
            clear = true;
          }

          ImGui::Separator(); // Grid related

          // Grid resolution
          if(ImGui::InputInt("Grid resolution", &resolution)){
            recompute_mesh = true;
          }

          // Boundary behaviour
          ImGui::Combo("Boundary behaviour", &boundary_item, "Reflect\0Radiate\0\0");

          // Wall type
          ImGui::Combo("Wall type", &wall_item, "None\0Single Slit\0Double Slit\0Custom\0\0");

          // Refractive index
          if(ImGui::Button("Change refractive index")){
            refractive_visible ^= 1;
          }

          if(refractive_visible){
            ImGui::Begin("Refractive Index");
            ImGui::Text("Paint obstructions on the white grid.");
            ImGui::End();
          }
        }

        ImGui::End();
      });
  }
}

void WaveGPUSimulation::receive(const event::ActiveInput& event){
  (void)event;
  if(event.has(InputAction::SPAWN_WAVELET)){
    spawn_drop = true;
  }
}

}
