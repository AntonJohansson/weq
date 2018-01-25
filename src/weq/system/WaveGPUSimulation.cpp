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
#include <cmath>

#include <weq/vars/Vars.hpp>
#include <weq/event/Internal.hpp>
#include <weq/event/DebugDraw.hpp>

using component::Renderable;
using component::WaveGPU;

namespace{
  bool clear_wave = false;
  bool clear_ri = false;
  bool set_c = false;
  bool resolution_changed = true; // Generate a mesh with default resolution and mesh_size.
  bool should_paint_grid = false;
  bool should_raycast = false;
  bool should_draw_brush_size = false;
  bool change_grid = false;

  int wall_item = 0; // 0 - none, 1 - single, 2, - double, 3 - custom
  int dt_item = 0; // 0 - variable dt, 1 - fix dt
  int boundary_item = 0; // 0 - reflect, 1 - radiate

  // Change refractive index
  int brush_size_percent = 16;
  float brush_refractive_index = 1.0f;

  //int resolution = 1000;
  int resolution = 100;

  float mesh_size = 5.0f;
  float c = 0.2f;
  float droplet_amplitude = 0.1;
  float droplet_sigma = 0.01;
  float safetfy_factor = 0.7;

  glm::vec3 spawn_drop_pos;

  std::tuple<glm::vec3, bool> ray_pick(glm::vec2 clip_coords, component::Camera camera, component::Transform transform){
    // Convert viewport coord to a ray in world coords
    // http://antongerdelan.net/opengl/raycasting.html
    glm::vec4 ray_clip = {clip_coords.x, clip_coords.y, -1.0f, 1.0f};
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

      return {intersect, true};
    }

    return {{0,0,0}, false};
  }

  void bound_c(double dt){
    float max_c = safetfy_factor*(mesh_size/resolution)/dt;
    if(c < 0.0f)c = 0.0f;
    if(c > max_c) c = max_c;
  }

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
  std::shared_ptr<gl::ShaderProgram> scene_grid_shader;
  std::shared_ptr<gl::ShaderProgram> scene_wave_shader;
  std::shared_ptr<Texture> grid_texture;
  bool spawn_drop = false;
  bool spawn_ray = false;

  glm::vec2 mouse;
}

void WaveGPUSimulation::configure(ex::EventManager& events){
  events.subscribe<event::ActiveInput>(*this);

  // Setup Shaders

  // Scene grid shader
  auto scene_grid_v = std::make_shared<gl::Shader>("scene_grid.vert");
  auto scene_grid_f = std::make_shared<gl::Shader>("scene_grid.frag");
  scene_grid_v->load();
  scene_grid_f->load();
  scene_grid_shader = std::make_shared<gl::ShaderProgram>("scene_grid.prog",
                                                     scene_grid_v,
                                                     scene_grid_f);
  scene_grid_shader->load();
  scene_grid_shader->link();

  // Scene wave shader
  auto scene_wave_v = std::make_shared<gl::Shader>("scene_wave.vert");
  auto scene_wave_f = std::make_shared<gl::Shader>("scene_wave.frag");
  scene_wave_v->load();
  scene_wave_f->load();
  scene_wave_shader = std::make_shared<gl::ShaderProgram>("scene_wave.prog",
                                                          scene_wave_v,
                                                          scene_wave_f);
  scene_wave_shader->load();
  scene_wave_shader->link();

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
  grid_texture = std::make_shared<Texture>("GridTexture", GL_TEXTURE_2D, resolution, resolution, GL_R32F, GL_RED, GL_FLOAT, nullptr);
  grid_texture->set_parameters({
      {GL_TEXTURE_BASE_LEVEL, 0},
      {GL_TEXTURE_MAX_LEVEL, 0},

      {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
      {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},

      {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
      {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
    });
  grid_texture->load();

  // Clear the grid paint texture on startup
  clear_ri = true;
}

void WaveGPUSimulation::update(ex::EntityManager& entities,
                            ex::EventManager& events,
                            ex::TimeDelta dt){
  (void)dt;

  // Add UI if it doesn't exist (!)
  add_ui(entities, events);

  if(should_raycast){
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

    auto[intersect, success] = ray_pick({mouse.x, mouse.y}, camera, transform);

    if(success && should_draw_brush_size){
      int brush_size = std::round(((float)brush_size_percent/100.0f)*resolution);
      glm::vec3 center = (resolution/mesh_size)*(intersect + glm::vec3(mesh_size/2.0f, mesh_size/2.0f, 0.0f));
      events.emit(event::DebugCircle((brush_size/2.0f)*(mesh_size/resolution), intersect + glm::vec3{0,0,0.1f}, {1,0,0,1}, 0.0f));
    }

    if(success &&
       spawn_drop &&
       glm::abs(intersect.x) <= mesh_size/2.0f &&
       glm::abs(intersect.y) <= mesh_size/2.0f){
      spawn_drop_pos = intersect;
      should_raycast = false;
    }

    // if inside mesh boundaries
    if(success &&
       should_paint_grid &&
       glm::abs(intersect.x) <= mesh_size/2.0f &&
       glm::abs(intersect.y) <= mesh_size/2.0f){

      // radius stuff
      int brush_size = std::round(((float)brush_size_percent/100.0f)*resolution);
      glm::vec3 center = (resolution/mesh_size)*(intersect + glm::vec3(mesh_size/2.0f, mesh_size/2.0f, 0.0f));
      float radius2 = std::pow(brush_size/2.0f, 2);

      glm::vec3 bottom_left = center - glm::vec3(brush_size/2, brush_size/2, 0);


      // Bound the rectangle
      int min_sub_size = 16;
      int sub_size = std::max(brush_size, min_sub_size);

      //Bound to texture
      if(bottom_left.x < 0){
       bottom_left.x = 0;
      }
      if(bottom_left.y < 0){
       bottom_left.y = 0;
      }
      if(bottom_left.x + sub_size > grid_texture->width()){
       bottom_left.x = grid_texture->width() - sub_size;
      }
      if(bottom_left.y + sub_size > grid_texture->height()){
       bottom_left.y = grid_texture->height() - sub_size;
      }

      unsigned int buffer_size = (sub_size)*(sub_size);
      //GLubyte* bits = new GLubyte[buffer_size];
      float* bits = new float[buffer_size];
      grid_texture->get_subdata((void*)bits, buffer_size*sizeof(float), bottom_left.x, bottom_left.y, sub_size, sub_size);

      // Encode the constant r = v*v/(gridsize*gridsize), gridsize = mesh_size/resolution, v = c/n
      //float v = c/brush_refractive_index;
      //float gridsize = mesh_size/resolution;
      //float r = v*v/(gridsize*gridsize);
      // Encode the refractive index in the texture
      for(int x = 0; x < sub_size; x++){
        for(int y = 0; y < sub_size; y++){
          float dx = glm::abs(x - (center.x - bottom_left.x));
          float dy = glm::abs(y - (center.y - bottom_left.y));
          if(dx*dx + dy*dy <= radius2){
            //bits[4*(x + y*sub_size) + 0] = std::round((255.0f/2.0f)*brush_refractive_index);
            //bits[4*(x + y*sub_size) + 1] = 0;
            //bits[4*(x + y*sub_size) + 2] = 0;
            //bits[4*(x + y*sub_size) + 3] = 255;
            bits[(x + y*sub_size)] = brush_refractive_index;
          }
        }
      }

      grid_texture->set_subdata(bottom_left.x, bottom_left.y, sub_size, sub_size, (void*)bits);
      delete[] bits;
      should_paint_grid = false;
    }
  }


  // Components
  entities.each<WaveGPU, Renderable>([dt](ex::Entity e, WaveGPU& wave, Renderable& r){
      (void)e;

      // Recompute grid mesh from new resolution
      if(resolution_changed){
        // Update grid paint textures size
        grid_texture->resize(resolution, resolution);

        // Update grid mesh
        wave.width = resolution;
        wave.height = resolution;
        wave.gridsize = mesh_size/resolution;
        wave.set_c(wave.c);
        wave.height_fbo.resize(resolution, resolution);
        wave.vel_fbo.resize(resolution, resolution);
        wave.edge_fbo.resize(resolution, 4);


        clear_wave = true;
        clear_ri = true;
        set_c = true;
        resolution_changed = false;

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
      if(set_c){bound_c(dt); wave.set_c(c); set_c = false;}

      // Clear waves on surface
      if(clear_wave){
        // Apply clear shader on height_fbo, vel_fbo, and edge_ebo
        clear_shader->use();
        wave.vel_fbo.texture()->bind();
        apply_shader(screen_mesh, wave.vel_fbo, clear_shader);

        wave.height_fbo.texture()->bind();
        apply_shader(screen_mesh, wave.height_fbo, clear_shader);

        wave.edge_fbo.texture()->bind();
        apply_shader(screen_mesh, wave.edge_fbo, clear_shader);

        clear_wave = false;
      }

      // Clear refractive index paint
      if(clear_ri){
        // Clear paint texture
        int w = grid_texture->width();
        int h = grid_texture->height();
        float* bits = new float[w*h];
        for(int i = 0; i < w*h; i++)bits[i] = 1.0f;
        grid_texture->set_subdata(0,0,w,h,bits);
        delete[] bits;

        clear_ri = false;
      }

      // Apply edge shader if boundary mode is set to radiate.
      if(boundary_item == 1){ // Radiate
        glViewport(0, 0, wave.width, 4);
        edge_shader->use();
        edge_shader->set("height_field", 0);
        edge_shader->set("edge_field", 1);
        edge_shader->set("ri_field", 2);
        edge_shader->set("gridsize", glm::vec2(wave.gridsize));
        edge_shader->set("pixelsize", glm::vec2(1.0/wave.width, 1.0/wave.height));
        edge_shader->set("c", wave.c);
        edge_shader->set("dt", dt);
        wave.height_fbo.texture()->bind(0);
        wave.edge_fbo.texture()->bind(1);
        grid_texture->bind(2);

        apply_shader(screen_mesh, wave.edge_fbo, edge_shader);
        glViewport(0, 0, wave.width, wave.height);
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
      vel_shader->set("ri_field", 2);
      vel_shader->set("edge_field", 3);
      vel_shader->set("dt", dt);
      vel_shader->set("pixelsize", glm::vec2(1.0/wave.width, 1.0/wave.height));
      vel_shader->set("gridsize", glm::vec2(wave.gridsize, wave.gridsize));
      vel_shader->set("c", wave.c);

      //temp
      vel_shader->set("radiate_edge", boundary_item == 1);

      wave.height_fbo.texture()->bind(0);
      wave.vel_fbo.texture()->bind(1);
      grid_texture->bind(2);
      wave.edge_fbo.texture()->bind(3);

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


      // Set the correct scene shader
      if(change_grid){
        // r = v*v/(gridsize*gridsize), v = c/n
        // r = c^2/(gridsize^2*n*2)
        // n^2 = c^2/(r*gridsize^2)
        //float k = wave.c*wave.c/(wave.gridsize*wave.gridsize);
        r.scene = scene_grid_shader;
        r.scene->use();
        r.scene->set("ri_field", 0);
        //r.scene->set("k", k);
        r.textures.push_back(grid_texture);
      }else{
        r.scene = scene_wave_shader;
        r.scene->use();
        r.scene->set("height_field", 0);
        r.textures.push_back(wave.height_fbo.texture());
      }

      // Visualize the output from each shader.
      ImGui::Begin("Debug");
        ImGui::Text("Output from velocity shader:");
        ImGui::Image((void*)wave.vel_fbo.texture()->handle(), ImVec2(200, 200));
        ImGui::Text("Output from height shader:");
        ImGui::Image((void*)wave.height_fbo.texture()->handle(), ImVec2(200, 200));
        ImGui::Text("Edge");
        ImGui::Image((void*)wave.edge_fbo.texture()->handle(), ImVec2(200, 32));
        ImGui::Text("Refractive index");
        ImGui::Image((void*)grid_texture->handle(), ImVec2(200, 200));
      ImGui::End();
    });
}

void WaveGPUSimulation::add_ui(ex::EntityManager& entities, ex::EventManager& events){
  if(!_ui_created){
    _ui_created = true;
    _ui = entities.create();
    _ui.assign<component::ImGui>([&](ex::EventManager& e){
        ImGui::Begin("Menu");

        ImGui::Separator();
        if(ImGui::InputFloat("Wave velocity (c)", &c)){
          bound_c(0.016);
          set_c = true;
        }

        if(ImGui::Button("Clear waves")){
          clear_wave = true;
        }

        ImGui::Separator(); // Droplet related

        if(ImGui::InputFloat("Droplet amplitude", &droplet_amplitude)){
        }

        if(ImGui::InputFloat("Droplet width", &droplet_sigma)){
        }

        ImGui::Separator(); // Grid related

        // Grid resolution
        if(ImGui::InputInt("Grid resolution", &resolution)){
          if(resolution < 16)resolution = 16;
          if(resolution > 2000)resolution = 2000;
          resolution_changed = true;
        }

        // Boundary behaviour
        ImGui::Combo("Boundary behaviour", &boundary_item, "Reflect\0Radiate\0\0");

        // Wall type
        ImGui::Combo("Wall type", &wall_item, "None\0Single Slit\0Double Slit\0Custom\0\0");

        // Refractive index
        if(ImGui::CollapsingHeader("Refractive index")){
          change_grid = true;
          should_raycast = true;
          should_draw_brush_size = true;

          ImGui::SliderInt("Paint brush radius (%)", &brush_size_percent, 1, 100);
          ImGui::SliderFloat("Refractive index (n)", &brush_refractive_index, 1.0f, 5.0f);

          if(ImGui::Button("Clear refractive index")){
            clear_ri = true;
          }
        }else{
          if(change_grid == true){
            change_grid = false;
            should_raycast = false;
            should_draw_brush_size = false;
          }
        }

        if(ImGui::CollapsingHeader("Settings")){
          ImGui::Combo("How to handle high velocities", &dt_item, "Variable dt, allow any c\0Fix dt, maximum c depending on gridsize\0\0");
        }

        ImGui::Separator(); // Exit

        // Handle exiting correctly :/
        if(ImGui::Button("Exit")){
          e.emit(event::Quit());
        }


        ImGui::End();
      });
  }
}

void WaveGPUSimulation::receive(const event::ActiveInput& event){
  if(event.has(InputState::SPAWN_RAY)){
    should_raycast = true;
    if(change_grid){
      should_paint_grid = true;
    }else{
      spawn_drop = true;
    }

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
