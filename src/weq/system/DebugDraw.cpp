#include <weq/system/DebugDraw.hpp>

#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>

#include <weq/event/DebugDraw.hpp>

#include <weq/component/Transform.hpp>
#include <weq/component/Renderable.hpp>

#include <weq/Mesh.hpp>

#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>

#include <weq/primitive/Vector.hpp>
#include <weq/primitive/Ray.hpp>
#include <weq/primitive/Circle.hpp>
#include <weq/primitive/Sphere.hpp>

#include <spdlog/spdlog.h>

#include <algorithm>

namespace weq::system{

const std::string vertex_source = R"(
#version 410

in vec3 position;
in vec3 normal;
in vec4 color;

out vec4 v_color;
out vec3 v_normal;

uniform mat4 mvp;

void main(){
  gl_Position = mvp * vec4(position, 1.0);
  v_color  = color;
  v_normal = normal;
}
)";

const std::string fragment_source = R"(
#version 410

in vec4 v_color;
in vec3 v_normal;

out vec4 frag_color;

void main(){
  frag_color = v_color + vec4(v_normal, 1.0);
}
)";


DebugDraw::DebugDraw()
  : _shader(nullptr){
}

DebugDraw::~DebugDraw(){
}

void DebugDraw::configure(EventManager& events){
  System<DebugDraw>::configure(events);
  // Events
  events.subscribe<event::DebugVector>(*this);
  events.subscribe<event::DebugRay>(*this);
  events.subscribe<event::DebugCircle>(*this);
  events.subscribe<event::DebugSphere>(*this);

  auto passthrough_v = std::make_shared<gl::Shader>(gl::ShaderType::VERTEX, vertex_source);
  auto passthrough_f = std::make_shared<gl::Shader>(gl::ShaderType::FRAGMENT, fragment_source);
  passthrough_v->load();
  passthrough_f->load();

  _shader = std::make_shared<gl::ShaderProgram>(passthrough_v, passthrough_f);
  _shader->load();
  _shader->link();
}

void DebugDraw::update(EntityManager& entities,
                       EventManager& events,
                       f32 dt){
  // Update entity durations
  for(auto itr = _timed_entities.begin(); itr != _timed_entities.end();){
    itr->second -= dt;
    if(itr->second < 0.0f){
      // Destory entity
      entities.destroy(itr->first);
      //itr->first.destroy();
      itr = _timed_entities.erase(itr);
    }else{
      ++itr;
    }
  }

  // Create new entities
  for(auto& pair : _buffered_events){
    auto& event = pair.first;
    auto& mesh = pair.second;

    // @TODO change to call on entitym
    auto e = entities.create();
    entities.assign<component::Transform>(e)->_position = event.position;
    entities.assign<component::Renderable>(e, mesh)->scene = _shader;
    //ke.assign<component::Transform>()->_position = event.position;
    //ke.assign<component::Renderable>(mesh)->scene = _shader;

    if(event.has_duration){
      _timed_entities.push_back({e, event.duration});
    }else{
      _persistent_entities.push_back(e);
    }
  }

  _buffered_events.clear();
}

void DebugDraw::receive(event::DebugVector& event){
  auto mesh = std::make_shared<Mesh>(primitive::vector::solid(event.vector, event.color), gl::DrawMode::LINES);
  _buffered_events.push_back({event, mesh});
}

void DebugDraw::receive(event::DebugRay& event){
  auto mesh = std::make_shared<Mesh>(primitive::ray::solid(event.direction, event.color), gl::DrawMode::LINES);
  _buffered_events.push_back({event, mesh});
}

void DebugDraw::receive(event::DebugCircle& event){
  auto mesh = std::make_shared<Mesh>(primitive::circle::dotted(event.radius, event.color), gl::DrawMode::LINES);
  _buffered_events.push_back({event, mesh});
}

void DebugDraw::receive(event::DebugSphere& event){
  auto mesh = std::make_shared<Mesh>(primitive::sphere::uv(event.radius, event.parallels, event.meridians), gl::DrawMode::TRIANGLES);
  _buffered_events.push_back({event, mesh});
}


}
