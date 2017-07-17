#include <weq/system/WaveSimulation.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Wave.hpp>
#include <weq/event/RegisterInput.hpp>

#include <algorithm>
#include <functional>
#include <deque>
#include <iostream>
#include <cmath>

using component::Renderable;
using component::Wave;

namespace{
  int d = 2;
  int b = 2;

  std::deque<std::function<void(FlatBuffer<float>& buffer, float t)>> filters;
  std::deque<std::function<bool(FlatBuffer<float>& buffer, int i, int j)>> obstructions;
  auto spawn_plane_wave = [](FlatBuffer<float>& buffer, float t){
    for(int j = 0; j < buffer.height(); j++){
      buffer(0,j) = 0.125f * std::sin(t/0.25f);
    }
  };

  auto single_slit = [](FlatBuffer<float>& buffer, int i, int j) -> bool{
    if(i == buffer.width()/3 && (j < buffer.height()/2 - 1 || j > buffer.height()/2 + 1)){
      return true;
    }else{
      return false;
    }
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

  auto clear_waves = [](FlatBuffer<float>& buffer, float t){
    buffer.fill(0);
    filters.clear();
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
  accumulated_time += dt;

  entities.each<Renderable, Wave>([dt](ex::Entity e,
                                       Renderable& renderable,
                                       Wave& w){
      for(auto& filter : filters){
        filter(w.height_field, accumulated_time);
      }

      float l, r, n , s, f, result;
      for(int i = 0; i <= w.width; i++){
        for(int j = 0; j <= w.height; j++){
          l = w.height_field(std::max(i-1, 0), j);
          r = w.height_field(std::min(i+1, w.width), j);
          n = w.height_field(i, std::min(j+1, w.height));
          s = w.height_field(i, std::max(j-1, 0));

          f = w.r * (l + r + n + s
                     - 4.0f*w.height_field(i,j));

          w.delta(i,j) += f*dt;
          //w.delta(i,j) *= 0.999f;

          for(auto& ob : obstructions){
            if(ob(w.height_field, i, j)){
              renderable.mesh->color_r(w.to_index(i,j)) = 1.0f;
              renderable.mesh->color_g(w.to_index(i,j)) = 0.0f;
              renderable.mesh->color_b(w.to_index(i,j)) = 0.0f;

              w.height_field(i,j) = 0.0f;
              w.delta(i,j) = 0.0f;
              break;
            }else{
              result = w.height_field(i,j) + w.delta(i,j)*dt;
              auto color_val_r = glm::abs(result/0.5f);
              auto color_val_g = glm::abs(result/0.25f);
              renderable.mesh->color_r(w.to_index(i,j)) = color_val_r;
              renderable.mesh->color_g(w.to_index(i,j)) = color_val_g;
              renderable.mesh->color_b(w.to_index(i,j)) = 0.1f;
            }
          }

          result = w.height_field(i,j) + w.delta(i,j)*dt;
          renderable.mesh->vertex_z(w.to_index(i,j)) = result;
        }
      }

      glm::vec3 normal;
      for(int i = 0; i <= w.width; i++){
        for(int j = 0; j <= w.height; j++){
          // Height's for surrounding vertices
//          l = renderable.mesh->vertex_z(w.to_index(std::max(i-1,0),j));
//          r = renderable.mesh->vertex_z(w.to_index(std::min(i+1,w.width),j));
//          n = renderable.mesh->vertex_z(w.to_index(i,std::min(j+1,w.height)));
//          s = renderable.mesh->vertex_z(w.to_index(i,std::max(j-1,0)));
//
          result = renderable.mesh->vertex_z(w.to_index(i,j));

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

//          normal = {l - r,
//                    s - n,
//                    r - l + 2.0f*w.gridsize};
//
//          normal = glm::normalize(normal);
//
//          renderable.mesh->normal_x(w.to_index(i,j)) = normal.x;
//          renderable.mesh->normal_y(w.to_index(i,j)) = normal.y;
//          renderable.mesh->normal_z(w.to_index(i,j)) = normal.z;
//
          w.height_field(i,j) = result;
        }
      }
    });
}

void WaveSimulation::receive(const event::ActiveInput& event){
  if(event.has(InputAction::SPAWN_PLANE_WAVE)){
    filters.push_front(spawn_plane_wave);
  }if(event.has(InputAction::CLEAR)){
  }if(event.has(InputAction::SINGLE_SLIT)){
    obstructions.push_front(single_slit);
  }if(event.has(InputAction::DOUBLE_SLIT)){
    obstructions.push_front(double_slit);
  }if(event.has(InputAction::INCREASE_D)){
    d+=2;
  }if(event.has(InputAction::DECREASE_D)){
    d-=2;
  }if(event.has(InputAction::INCREASE_B)){
    b++;
  }if(event.has(InputAction::DECREASE_B)){
    b--;
  }
}

}
