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

using component::Renderable;
using component::WaveGPU;

// TODO change format of texture buffers.

namespace weq::system{

namespace{
  std::shared_ptr<Mesh> screen_mesh;
  std::shared_ptr<gl::ShaderProgram> force_shader;
}

void WaveGPUSimulation::configure(ex::EventManager& events){
  events.subscribe<event::ActiveInput>(*this);

  // Setup Shaders
  // Force calculation shader
  auto force_v = std::make_shared<gl::Shader>("force.vert");
  auto force_f = std::make_shared<gl::Shader>("force.frag");
  force_v->load();
  force_f->load();
  force_shader = std::make_shared<gl::ShaderProgram>("force.prog",
                                                     force_v,
                                                     force_f);
  force_shader->load();
  force_shader->link();

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
  screen_mesh->generate_vao(force_shader);
}

void WaveGPUSimulation::update(ex::EntityManager& entities,
                            ex::EventManager& events,
                            ex::TimeDelta dt){

  (void)events;
  (void)dt;

  entities.each<WaveGPU, Renderable>([](ex::Entity e, WaveGPU& wave, Renderable& r){
      (void)e;

      wave.force_fbo.bind();

      // Retrive old viewport settings.
      GLint old_viewport[4];
      glGetIntegerv(GL_VIEWPORT, old_viewport);

      glViewport(0, 0, wave.width, wave.height);

      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      force_shader->use();
      force_shader->set("height_field", 0);
      force_shader->set("gridsize", wave.gridsize);
      force_shader->set("r", wave.r);

      wave.force_fbo.texture()->bind();

      // bind mesh
      screen_mesh->vao(force_shader->id()).bind();
      screen_mesh->ebo().bind();
      //r.mesh->vao(wave.force_shader->id()).bind();
      //r.mesh->ebo().bind();

      //glDrawElements(GLenum(r.mesh->draw_mode()),
      //             r.mesh->ebo().size(), GL_UNSIGNED_INT, 0);
      glDrawElements(GLenum(screen_mesh->draw_mode()),
                     screen_mesh->ebo().size(), GL_UNSIGNED_INT, 0);

      wave.force_fbo.unbind();

      // Reset viewport settings.
      glViewport(old_viewport[0],
                 old_viewport[1],
                 old_viewport[2],
                 old_viewport[3]);

      r.texture = wave.force_fbo.texture();
    });
}

void WaveGPUSimulation::receive(const event::ActiveInput& event){
  (void)event;
}

}
