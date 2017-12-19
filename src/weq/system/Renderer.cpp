#include <weq/system/Renderer.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Camera.hpp>
#include <weq/event/Input.hpp>
#include <weq/event/Window.hpp>
#include <weq/gl/VertexFormat.hpp>
#include <weq/gl/Framebuffer.hpp>
#include <weq/Window.hpp>
#include <weq/Texture.hpp>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <spdlog/spdlog.h>

namespace weq::system{

namespace{
std::shared_ptr<Texture> texture;
std::shared_ptr<gl::ShaderProgram> screen_p;
std::shared_ptr<Mesh> screen_mesh;
std::shared_ptr<gl::Framebuffer> scene_fbo;
glm::mat4 view;
glm::mat4 proj;
glm::mat4 model;
}

using component::Renderable;
using component::Transform;

Renderer::Renderer(){
}

Renderer::~Renderer(){
}

void Renderer::configure(ex::EventManager& events){
  spdlog::get("console")->info("renderer");
  // Events
  events.subscribe<event::ActiveInput>(*this);
  events.subscribe<event::ActiveWindow>(*this);

  // Setup screen shader
  auto screen_v = std::make_shared<gl::Shader>("screen.vert");
  screen_v->load();
  auto screen_f = std::make_shared<gl::Shader>("screen.frag");
  screen_f->load();
  screen_p = std::make_shared<gl::ShaderProgram>("screen.prog",
                                                 screen_v,
                                                 screen_f);
  screen_p->load();
  screen_p->link();

  // Setup screen mesh
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
  screen_mesh->generate_vao(screen_p);


  // Setup cubemap
  texture = std::make_shared<Texture>("cloudtop_bk.tga", GL_TEXTURE_2D);
  texture->load();
  texture->set_parameters({
      {GL_TEXTURE_BASE_LEVEL, 0},
      {GL_TEXTURE_MAX_LEVEL, 0},

      {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
      {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},

      {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
      {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
      });
}

void Renderer::update(ex::EntityManager& entities,
                      ex::EventManager& events,
                      ex::TimeDelta dt){

  (void)events;

  static glm::mat4 mvp; // should prob not be static (mem.?).
  static glm::mat4 tmp_model;

  // Get the active camera (TODO must be a better way).
  component::Camera active_camera;
  entities.each<component::Camera, component::ActiveCamera>([&active_camera](ex::Entity e, component::Camera& c, component::ActiveCamera& a){active_camera = c;});

  // caluclate vp-matrix
  active_camera.viewproj = active_camera.projection * active_camera.view;
  view = active_camera.view;
  proj = active_camera.projection;

  // Bind scene fbo.
  scene_fbo->bind();

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  // Move draw code out of entities loop, works fine since there's only a
  // single entity.

  entities.each<Renderable, Transform>([dt, &active_camera](ex::Entity e,
                                                            Renderable& r,
                                                            Transform& t){
      // Draw the mesh if it is drawable.
      if(r.mesh->is_valid()){
        tmp_model = t.model();
        // calculate mvp for each model
        mvp = active_camera.viewproj * t.model();
        r.scene->use();
        r.scene->set("mvp", mvp);
        r.scene->set("normal_matrix", active_camera.normal_matrix);
        r.scene->set("height_field", 0);

        for(int i = 0; i < r.textures.size(); i++){
          auto ptr = r.textures[i];
          if(ptr){
            ptr->bind(i);
          }
        }

        r.mesh->vao(r.scene).bind();
        r.mesh->ebo().bind();

        glDrawElements(GLenum(r.mesh->draw_mode()),
                       r.mesh->ebo().size(), GL_UNSIGNED_INT, 0);
      }
    });

  model = tmp_model;

  glDisable(GL_DEPTH_TEST);
  scene_fbo->unbind();

  // Draw screen from fbo

  screen_p->use();
  screen_p->set("framebuffer", 0);

  screen_mesh->vao(screen_p).bind();
  screen_mesh->ebo().bind();

  scene_fbo->texture()->bind(0);

  glDrawElements(GLenum(screen_mesh->draw_mode()),
                 screen_mesh->ebo().size(), GL_UNSIGNED_INT, 0);

  // Render UI
  ImGui::Render();

  _window->swap_buffers();

  // Check for OpenGL errors (TODO should be able to disable this?)
  GLenum err;
  while((err = glGetError()) != GL_NO_ERROR){
    spdlog::get("console")->error("GL-error: {}", err);
  }
}

void Renderer::receive(const event::ActiveInput& event){
  // Unproject doesn't work correctly.
  //if(event.has(InputRange::CURSOR_X) && event.has(InputRange::CURSOR_Y)){
  //  // Unproject mouse coords.
  //  scene_fbo->bind(); // Read depth from scene-fbo
  //  float x = event.ranges.at(InputRange::CURSOR_X);
  //  float y = event.ranges.at(InputRange::CURSOR_Y);
  //  float z = scene_fbo->depth(x*_window->width(), y*_window->height());
  //  glm::vec3 win = {x*_window->width(), y*_window->height(), z};
  //  auto vec = glm::unProject(win, view*model, proj, glm::vec4{0, 0, _window->width(), _window->height()});
  //  //spdlog::get("console")->info("{}, {}, {} - depth {}", vec.x, vec.y, vec.z, z);
  //  scene_fbo->unbind();
  //}

  if(event.has(InputState::CURSOR_DOWN)){
    _window->set_cursor_mode(CursorMode::DISABLED);

  }else{
    // TODO should probably sent out a "state-changed" event
    _window->set_cursor_mode(CursorMode::NORMAL);
  }

  // Set render mode (TODO remove?)
  if(event.has(InputAction::RENDER_WIREFRAME)){
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }else if(event.has(InputAction::RENDER_SOLID)){
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }
}

void Renderer::receive(const event::ActiveWindow& event){
  _window = event.window;
  configure_scene_fbo();
}

// Private functions

void Renderer::configure_scene_fbo(){
  scene_fbo = std::make_shared<gl::Framebuffer>(_window->width(), _window->height());
  scene_fbo->texture()->set_parameters({
      {GL_TEXTURE_BASE_LEVEL, 0},
      {GL_TEXTURE_MAX_LEVEL, 0},

      {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
      {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},

      {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
      {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
      });
}

} // namespace weq::system
