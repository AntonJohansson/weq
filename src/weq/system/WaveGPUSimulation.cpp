#include <weq/system/WaveGPUSimulation.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Wave.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Camera.hpp>
#include <weq/component/Transform.hpp>
#include <weq/event/Input.hpp>
#include <weq/event/DebugDraw.hpp>
//#include <weq/event/Camera.hpp>
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
#include <algorithm>

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
  float droplet_amplitude = 0.1;
  float droplet_sigma = 0.01;

  glm::vec3 spawn_drop_pos;

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
  std::shared_ptr<Texture> grid_texture;
  bool spawn_drop = false;
  bool spawn_ray = false;

  glm::vec2 mouse;
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

  // Grid texture
  grid_texture = std::make_shared<Texture>("GridTexture", GL_TEXTURE_2D, 1000, 1000, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  grid_texture->set_parameters({
      {GL_TEXTURE_BASE_LEVEL, 0},
      {GL_TEXTURE_MAX_LEVEL, 0},

      {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
      {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},

      {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
      {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
    });
  grid_texture->load();
}

void WaveGPUSimulation::update(ex::EntityManager& entities,
                            ex::EventManager& events,
                            ex::TimeDelta dt){
  (void)events;
  (void)dt;

  // Add UI if it doesn't exist (!)
  add_ui(entities);

  if(spawn_ray){
    // Get the currently active camera
    component::Transform transform;
    component::Camera camera;
    entities.each<component::Camera,
                  component::ActiveCamera,
                  component::Transform>(
                    [&camera, &transform](ex::Entity e,
                                 component::Camera c,
                                 component::ActiveCamera& a,
                                 component::Transform& t){
                      camera = c;
                      transform = t;
                    });

    // Convert viewport coord to a ray in world coords
    // http://antongerdelan.net/opengl/raycasting.html
    glm::vec4 ray_clip = {mouse.x, mouse.y, -1.0f, 1.0f};
    glm::vec4 ray_eye = glm::inverse(camera.projection) * ray_clip;
    ray_eye = {ray_eye.x, ray_eye.y, -1.0, 0.0};

    glm::vec3 ray_world = glm::vec3(glm::inverse(camera.view) * ray_eye);
    ray_world = glm::normalize(ray_world);

    // Ray plane intersection

    glm::vec3 plane_normal = {0, 0, 1};

    float dot = glm::dot(ray_world, plane_normal);
    if(dot != 0){
      float d = 0.0f;
      float t = - (glm::dot(transform._position, plane_normal) + d) / dot;
      glm::vec3 intersect = transform._position + t * ray_world;

      // if inside mesh boundaries
      if(glm::abs(intersect.x) <= mesh_size/2.0f &&
         glm::abs(intersect.y) <= mesh_size/2.0f){
        spawn_drop = true;
        spawn_drop_pos = intersect;

        // radius stuff
        const unsigned int size = 40;
        float radius = size/2.0f;

        glm::vec3 center = (1000.0f/5.0f)*(spawn_drop_pos + glm::vec3(2.5f, 2.5f, 0.0f));
        glm::vec3 bottom_left = center - glm::vec3(size/2, size/2, 0);

        // Bound the rectangle
        int width = size;
        int height = size;
        if(bottom_left.x < 0){
          width = size + bottom_left.x;
          bottom_left.x = 0;
        }
        if(bottom_left.y < 0){
          height = size + bottom_left.y;
          bottom_left.y = 0;
        }
        if(bottom_left.x + size > grid_texture->width()){
          width = grid_texture->width() - bottom_left.x;
        }
        if(bottom_left.y + size > grid_texture->height()){
          height = grid_texture->height() - bottom_left.y;
        }

        //GLubyte* bits = new GLubyte[width*height*3];
        GLubyte bits[size*size*3] = {0};
        grid_texture->get_data(bits, size*size*3, bottom_left.x, bottom_left.y, width, height);
        spdlog::get("console")->info("{},{}\t {},{}", bottom_left.x, bottom_left.y, width, height);

        for(int x = 0; x < width; x++){
          for(int y = 0; y < height; y++){
            float dx = glm::abs(bottom_left.x + x - center.x);
            float dy = glm::abs(bottom_left.y + y - center.y);
            if(dx*dx + dy*dy <= radius){
              bits[3*(x*width + y) + 0] = 255;
              bits[3*(x*width + y) + 1] = 0;
              bits[3*(x*width + y) + 2] = 0;
            }
          }
        }

        grid_texture->set_data(bottom_left.x, bottom_left.y, width, height, (void*)bits);
		delete bits;
      }
    }

    spawn_ray = false;
  }

  // Components
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
        drop_shader->set("pos", glm::vec2(spawn_drop_pos)/mesh_size);
        drop_shader->set("amplitude", droplet_amplitude);
        drop_shader->set("sigma", droplet_sigma);
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
      r.textures.push_back(grid_texture);

      // Visualize the output from each shader.
      ImGui::Begin("Debug");
        ImGui::Text("Output from velocity shader:");
        ImGui::Image((void*)wave.vel_fbo.texture()->handle(), ImVec2(200, 200));

        ImGui::Text("Output from height shader:");
        ImGui::Image((void*)wave.height_fbo.texture()->handle(), ImVec2(200, 200));
        ImGui::Text("Wall");
        ImGui::Image((void*)grid_texture->handle(), ImVec2(200, 200));
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

          ImGui::Separator(); // Droplet related

          if(ImGui::InputFloat("Droplet amplitude", &droplet_amplitude)){
          }

          if(ImGui::InputFloat("Droplet width", &droplet_sigma)){
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
  if(event.has(InputAction::SPAWN_WAVELET)){
    spawn_drop = true;
  }
  if(event.has(InputState::SPAWN_RAY)){
    spawn_ray = true;
  }
  if(event.has(InputRange::CURSOR_X) && event.has(InputRange::CURSOR_Y)){
    mouse.x = event.ranges.at(InputRange::CURSOR_X);
    mouse.y = event.ranges.at(InputRange::CURSOR_Y);
  }
}

// There is another way to do this
//void WaveGPUSimulation::receive(const event::ActiveCamera& event){
//  if()
//}

}
