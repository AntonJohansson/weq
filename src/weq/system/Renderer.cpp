#include <weq/system/Renderer.hpp>

#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>

#include <weq/utility/NumberTypes.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/gl/VertexFormat.hpp>
#include <weq/gl/Framebuffer.hpp>
#include <weq/gl/Texture.hpp>
#include <weq/gl/Cubemap.hpp>

#include <weq/primitive/Cube.hpp>

#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Camera.hpp>

#include <weq/event/Input.hpp>
#include <weq/event/Window.hpp>
#include <weq/event/Hotloader.hpp>

#include <weq/Window.hpp>
#include <weq/memory/ResourceManager.hpp>

// @TODO TEMP
#include <weq/utility/GlmHelper.hpp>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <spdlog/spdlog.h>

namespace weq::system{

namespace{
std::shared_ptr<gl::Cubemap> cubemap;
std::shared_ptr<gl::ShaderProgram> screen_p;
std::shared_ptr<gl::ShaderProgram> skymap_p;
std::shared_ptr<Mesh> screen_mesh;
std::shared_ptr<Mesh> skymap_mesh;
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

void Renderer::configure(EventManager& events){
  System<Renderer>::configure(events);

  // Events
  events.subscribe<event::ActiveInput>(*this);
  events.subscribe<event::ActiveWindow>(*this);

  namespace rm = memory::resource_manager;

  // Setup screen shader
  screen_p = rm::load_shader_program("screen.prog");
  skymap_p = rm::load_shader_program("skymap.prog");

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
  auto skymap_mesh_data = primitive::cube::inwards_facing(20.0f);
  skymap_mesh = std::make_shared<Mesh>(skymap_mesh_data,
                                       gl::DrawMode::TRIANGLES);
  skymap_mesh->generate_vao(skymap_p);

  cubemap = rm::load_cubemap("miramar_ft.tga",
                             "miramar_bk.tga",
                             "miramar_up.tga",
                             "miramar_dn.tga",
                             "miramar_lf.tga",
                             "miramar_rt.tga");
}

void Renderer::update(EntityManager& entities,
                      EventManager& events,
                      f32 dt){

  (void)events;

  static glm::mat4 mvp; // should prob not be static (mem.?).
  static glm::mat4 tmp_model;

  // Get the active camera (TODO must be a better way).
  component::Camera active_camera;
  component::Transform active_camera_transform;
  entities.each<component::Transform, component::Camera, component::ActiveCamera>([&](EntityId e, component::Transform& t, component::Camera& c, component::ActiveCamera& a){
      (void)e;
      (void)a;
      active_camera = c;
      active_camera_transform = t;
    });

  // caluclate vp-matrix
  active_camera.viewproj = active_camera.projection * active_camera.view;
  view = active_camera.view;
  proj = active_camera.projection;

  // Bind scene fbo.
  scene_fbo->bind();

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  //glDepthFunc(GL_LEQUAL);
  //glDepthRange(0.0f, 1.0f);
  glClearColor(0, 0, 0, 1);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  // Draw skybox
  glEnable(GL_CULL_FACE);

  glDepthMask(GL_FALSE);
  skymap_p->use();
  // Rotate due to different axes :)
  auto skymap_m = glm::rotate(glm::mat4(), glm::half_pi<float>(), {1,0,0});
  // Converting view to mat3 -> mat4 removes translation component!
  auto skymap_vp = active_camera.projection * glm::mat4(glm::mat3(active_camera.view)) * skymap_m;
  skymap_p->set("vp", skymap_vp);
  screen_p->set("cube_texture", 0);

  skymap_mesh->vao(skymap_p).bind();
  skymap_mesh->ebo().bind();

  cubemap->bind(0);

  glDrawElements(GLenum(skymap_mesh->draw_mode()),
                 skymap_mesh->ebo().size(), GL_UNSIGNED_INT, 0);

  glDepthMask(GL_TRUE);

  // Assume entities won't need culling.
  // Naive as fuck @TODO fix
  glDisable(GL_CULL_FACE);


  // Move draw code out of entities loop, works fine since there's only a
  // single entity.
  entities.each<Renderable, Transform>([dt, &active_camera, &active_camera_transform](EntityId e,
                                                                                      Renderable& r,
                                                                                      Transform& t){
      (void)e;
      // Draw the mesh if it is drawable.
      if(r.mesh->is_valid()){
        tmp_model = t.model();
        // calculate mvp for each model
        mvp = active_camera.viewproj * t.model();
        //draw_mat("mvp", mvp);
        r.scene->use();
        r.scene->set("mvp", mvp);
        r.scene->set("normal_matrix", active_camera.normal_matrix);

        // Bind textures
        for(u32 i = 0; i < r.textures.size(); i++){
          auto ptr = r.textures[i];
          if(ptr){
            ptr->bind(i);
          }
        }

        // Bind skybox if requested
        if(r.require_skybox){
          r.scene->set("skybox", (int)r.textures.size());
          cubemap->bind(r.textures.size());
        }
        // Bind camera pos if requrested
        if(r.require_camera_pos){
          r.scene->set("camera_pos", active_camera_transform._position);
          r.scene->set("normal_mat", active_camera.normal_matrix);
          r.scene->set("camera_model", active_camera_transform.model());
          r.scene->set("model", t.model());
        }
        // Bind wireframe if requested
        if(r.wireframe){
          glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        }


        r.mesh->vao(r.scene).bind();
        r.mesh->ebo().bind();

        glDrawElements(GLenum(r.mesh->draw_mode()),
                       r.mesh->ebo().size(), GL_UNSIGNED_INT, 0);

        //ImGui::Begin("Debug");
        //ImGui::Text(r.scene->path().string().c_str());
        //ImGui::Image((void*)scene_fbo->texture()->handle(), ImVec2(200,200));
        //ImGui::End();

        // Unbind wireframe if it was requested
        if(r.wireframe){
          glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
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

  // Display
  _window->swap_buffers();

  // Check for OpenGL errors (TODO should be able to disable this?)
  GLenum err;
  while((err = glGetError()) != GL_NO_ERROR){
    spdlog::get("console")->error("GL-error: {}", err);
  }
}

void Renderer::receive(event::ActiveInput& event){
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

void Renderer::receive(event::ActiveWindow& event){
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
