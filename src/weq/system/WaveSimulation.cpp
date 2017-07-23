#include <weq/system/WaveSimulation.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Wave.hpp>
#include <weq/event/RegisterInput.hpp>

#include <spdlog/spdlog.h>

#include <glad/glad.h> //@TODO required?
#include <weq/Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <functional>
#include <deque>
#include <iostream>
#include <cmath>

using component::Renderable;
using component::Wave;

//@TODO SLIM DOWN SIMULATION AND ENGINE DESIGN,
//      I have a single entity, this is not requried!
//@TODO Gaussian distribution for water droplets?
//@TODO Split this file into smaller files

namespace{
  int d = 0;
  int b = 2;
  float c = 0.5f;
  float T = 0.25f;
  bool should_clear = false;
  bool update_color = false;
  bool should_reflect = true;

  std::deque<std::function<void(FlatBuffer<float>& buffer, float t)>> filters;
  std::deque<std::function<bool(FlatBuffer<float>& buffer, int i, int j)>> obstructions;
  auto spawn_plane_wave = [](FlatBuffer<float>& buffer, float t){
    for(int j = 0; j < buffer.height(); j++){
      buffer(0,j) = 0.125f * std::sin(t/T);
    }
  };

  auto spawn_wavelet = [](FlatBuffer<float>& buffer, float t){
    buffer(buffer.width()/2, buffer.height()/2) = 5.0f;
    filters.clear();
  };

  auto double_slit = [](FlatBuffer<float>& buffer, int i, int j) -> bool{
    if(i == buffer.width()/3
       && (j < buffer.height()/2 - b - d/2
           || j > buffer.height()/2 + b + d/2
           || (j > buffer.height()/2 - d/2 && j < buffer.height()/2 + d/2))){
      return true;
    }else{
      return false;
    }
  };
}

namespace weq::system{

void WaveSimulation::configure(ex::EventManager& events){
  events.subscribe<event::ActiveInput>(*this);
}

void WaveSimulation::update(ex::EntityManager& entities,
                            ex::EventManager& events,
                            ex::TimeDelta dt){

  static float accumulated_time = 0.0f;
  static float l, r, n, s, f, result;
  static glm::vec3 normal;

  accumulated_time += dt;

  entities.each<Renderable, Wave>([dt](ex::Entity e,
                                       Renderable& renderable,
                                       Wave& w){

      w.r = c*c/(w.gridsize*w.gridsize); //@TODO this is temporary
      for(auto& filter : filters){
        filter(w.height_field, accumulated_time);
      }

      for(int i = 0; i <= w.width; i++){
        for(int j = 0; j <= w.height; j++){
          // Spatial laplacian
          // For open boundaries update invisble column according to:
          // g_new = (c*dt*u + g*h)/(h + c*dt);
          // where u = column hight of (i,j) and h = gridsize

          if(should_clear){ //@TODO this only holds for a single entity
            w.height_field.fill(0);
            w.delta.fill(0);
            filters.clear();
            obstructions.clear();

            should_clear = false;
          }

          if(should_reflect){
            l = w.height_field(std::max(i-1, 0), j);
            r = w.height_field(std::min(i+1, w.width), j);
            n = w.height_field(i, std::min(j+1, w.height));
            s = w.height_field(i, std::max(j-1, 0));
          }else{
            // g_new = (c*dt*u + g*h)/(h + c*dt);
            if(i-1 < 0){
              l = w.l_edge[j] = (c*dt*w.height_field(i,j) + w.l_edge[j]*w.gridsize)/(w.gridsize + c*dt);
            }else{
              l = w.height_field(i-1, j);
            }

            if(i+1 > w.width){
              r = w.r_edge[j] = (c*dt*w.height_field(i,j) + w.r_edge[j]*w.gridsize)/(w.gridsize + c*dt);
            }else{
              r = w.height_field(i+1, j);
            }

            if(j+1 > w.height){
              n = w.n_edge[i] = (c*dt*w.height_field(i,j) + w.n_edge[i]*w.gridsize)/(w.gridsize + c*dt);
            }else{
              n = w.height_field(i, j+1);
            }

            if(j-1 < 0){
              s = w.s_edge[i] = (c*dt*w.height_field(i,j) + w.s_edge[i]*w.gridsize)/(w.gridsize + c*dt);
            }else{
              s = w.height_field(i, j-1);
            }
          }

          // Force
          f = w.r * (l + r + n + s
                     - 4.0f*w.height_field(i,j));

          // Change in height per unit of time
          w.delta(i,j) += f*dt;

          // Dampning
          if(false){
            w.delta(i,j) *= 0.999f;
          }

          // Normal calculation
          normal = {l - r,
                    s - n,
                    r - l + 2.0f*w.gridsize};

          normal = glm::normalize(normal);

          renderable.mesh->normal_x(w.to_index(i,j)) = normal.x;
          renderable.mesh->normal_y(w.to_index(i,j)) = normal.y;
          renderable.mesh->normal_z(w.to_index(i,j)) = normal.z;

          bool should_update_color = true;
          for(auto& ob : obstructions){
            if(ob(w.height_field, i, j)){
              renderable.mesh->color_r(w.to_index(i,j)) = 1.0f;
              renderable.mesh->color_g(w.to_index(i,j)) = 0.0f;
              renderable.mesh->color_b(w.to_index(i,j)) = 0.0f;

              w.height_field(i,j) = 0.0f;
              w.delta(i,j) = 0.0f;
              should_update_color = false;
              break;
            }
          }

          result = w.height_field(i,j) + w.delta(i,j)*dt;
          renderable.mesh->vertex_z(w.to_index(i,j)) = result;

          // Update color;
          if(update_color && should_update_color){
            auto color_val_1 = glm::abs(result/0.5f);
            auto color_val_2 = glm::abs(result/0.25f);
            auto color_val_3 = glm::abs(result/0.1f);

            if(result > 0 ){
              renderable.mesh->color_r(w.to_index(i,j)) = color_val_1;
              renderable.mesh->color_g(w.to_index(i,j)) = color_val_2;
              renderable.mesh->color_b(w.to_index(i,j)) = color_val_3;
            }else{
              renderable.mesh->color_r(w.to_index(i,j)) = color_val_3;
              renderable.mesh->color_g(w.to_index(i,j)) = color_val_2;
              renderable.mesh->color_b(w.to_index(i,j)) = color_val_1;
            }
          }
        }
      }

      for(int i = 0; i <= w.width; i++){
        for(int j = 0; j <= w.height; j++){
          w.height_field(i,j) = renderable.mesh->vertex_z(w.to_index(i,j));
        }
      }
    });
}

void WaveSimulation::receive(const event::ActiveInput& event){
  if(event.has(InputAction::SPAWN_PLANE_WAVE)){
    filters.push_front(spawn_plane_wave);
  }if(event.has(InputAction::SPAWN_WAVELET)){
    filters.push_front(spawn_wavelet);
  }if(event.has(InputAction::CLEAR)){
    should_clear = true;
  }if(event.has(InputAction::DOUBLE_SLIT)){
    obstructions.push_front(double_slit);
  }if(event.has(InputAction::INCREASE_D)){
    d+=2;
    spdlog::get("console")->info("d: {}", d);
  }if(event.has(InputAction::DECREASE_D)){
    d-=2;
    spdlog::get("console")->info("d: {}", d);
  }if(event.has(InputAction::INCREASE_B)){
    b++;
    spdlog::get("console")->info("b: {}", b);
  }if(event.has(InputAction::DECREASE_B)){
    b--;
    spdlog::get("console")->info("b: {}", b);
  }if(event.has(InputAction::UPDATE_COLOR)){
    update_color = true;
  }if(event.has(InputAction::TOGGLE_REFLECT)){
    should_reflect = !should_reflect;
    spdlog::get("console")->info("reflect: {}", should_reflect);
  }if(event.has(InputAction::INCREASE_C)){
    c += 0.05f;
    spdlog::get("console")->info("c: {}", c);
  }if(event.has(InputAction::DECREASE_C)){
    c -= 0.05f;
    spdlog::get("console")->info("c: {}", c);
  }if(event.has(InputAction::INCREASE_T)){
    T += 0.05f;
    spdlog::get("console")->info("T: {}", T);
  }if(event.has(InputAction::DECREASE_T)){
    T -= 0.05f;
    spdlog::get("console")->info("T: {}", T);
  }
}

}

// Normal Calculation derivation
// glm::vec3 lv = {-w.gridsize, 0, l - c};
// glm::vec3 lr = { w.gridsize, 0, r - c};
// glm::vec3 ln = {0, w.gridsize,  n - c};
// glm::vec3 ls = {0,-w.gridsize,  s - c};

//  x    y    z
//  lv.x lv.y lv.z
//  0    -1   0
//  z*lv.x*-1 - x*lv.z*-1
//  x*lv.z - z*lv.x
// glm::vec3 nv = glm::cross(lv, {0, -1, 0});
//  x    y    z
//  lv.x lv.y lv.z
//  0    1    0
//  z*lv.x*1 - x*lv.z*1
//  z*lv.x - x*lv.z
// glm::vec3 nr = glm::cross(lr, {0,  1, 0});
//  x    y    z
//  lv.x lv.y lv.z
//  -1   0    0
//  y*lv.z*-1 - z*lv.y*-1
//  z*lv.y - y*lv.z
// glm::vec3 nn = glm::cross(ln, {-1, 0, 0});
//  x    y    z
//  lv.x lv.y lv.z
//  1    0    0
//  y*lv.z*1 - z*lv.y*1
//  y*lv.z - z*lv.y
// glm::vec3 ns = glm::cross(ls, { 1, 0, 0});

// glm::vec3 nv = {lv.z, 0, -lv.z};
// glm::vec3 nr = {-lr.z, 0, lr.z};
// glm::vec3 nn = {0, -ln.z, ln.y};
// glm::vec3 ns = {0, ls.z, -ls.y};

// glm::vec3 lv = {-w.gridsize, 0, l - c};
// glm::vec3 lr = { w.gridsize, 0, r - c};
// glm::vec3 ln = {0, w.gridsize,  n - c};
// glm::vec3 ls = {0,-w.gridsize,  s - c};
//
// glm::vec3 n = glm::normalize(nv + nr + nn + ns);
