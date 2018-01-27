#include <weq/system/DebugDraw.hpp>
#include <weq/event/DebugDraw.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/Mesh.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/primitive/Vector.hpp>
#include <weq/primitive/Ray.hpp>
#include <weq/primitive/Circle.hpp>

#include <spdlog/spdlog.h>

#include <algorithm>


namespace weq::system{

const std::string vertex_source = R"(
#version 410

in vec3 position;
in vec4 color;

out vec4 v_color;

uniform mat4 mvp;

void main(){
  gl_Position = mvp * vec4(position, 1.0);
  v_color = color;
}
)";

const std::string fragment_source = R"(
#version 410

in vec4 v_color;

out vec4 frag_color;

void main(){
  frag_color = v_color;
}
)";


DebugDraw::DebugDraw()
  : _shader(nullptr){
}

DebugDraw::~DebugDraw(){
}

void DebugDraw::configure(ex::EventManager& events){
  spdlog::get("console")->info("debugdraw");
  // Events
  events.subscribe<event::DebugVector>(*this);
  events.subscribe<event::DebugRay>(*this);
  events.subscribe<event::DebugCircle>(*this);

  auto passthrough_v = std::make_shared<gl::Shader>(gl::ShaderType::VERTEX, vertex_source);
  auto passthrough_f = std::make_shared<gl::Shader>(gl::ShaderType::FRAGMENT, fragment_source);
  passthrough_v->load();
  passthrough_f->load();

  _shader = std::make_shared<gl::ShaderProgram>(passthrough_v, passthrough_f);
  _shader->load();
  _shader->link();
}

void DebugDraw::update(ex::EntityManager& entities,
            ex::EventManager& events,
            ex::TimeDelta dt){
  // Update entity durations
  for(auto itr = _timed_entities.begin(); itr != _timed_entities.end();){
    itr->second -= dt;
    if(itr->second < 0.0f){
      itr->first.destroy();
      itr = _timed_entities.erase(itr);
    }else{
      ++itr;
    }
  }

  // Create new entities
  for(auto& pair : _buffered_events){
    auto& event = pair.first;
    auto& mesh = pair.second;

    auto e = entities.create();
    e.assign<component::Transform>()->_position = event.position;
    e.assign<component::Renderable>(mesh)->scene = _shader;

    if(event.has_duration){
      _timed_entities.push_back({e, event.duration});
    }else{
      _persistent_entities.push_back(e);
    }
  }

  _buffered_events.clear();
}

void DebugDraw::receive(const event::DebugVector& event){
  auto mesh = std::make_shared<Mesh>(primitive::vector::solid(event.vector, event.color), gl::DrawMode::LINES);
  _buffered_events.push_back({event, mesh});
}

void DebugDraw::receive(const event::DebugRay& event){
  auto mesh = std::make_shared<Mesh>(primitive::ray::solid(event.direction, event.color), gl::DrawMode::LINES);
  _buffered_events.push_back({event, mesh});
}

void DebugDraw::receive(const event::DebugCircle& event){
  auto mesh = std::make_shared<Mesh>(primitive::circle::dotted(event.radius, event.color), gl::DrawMode::LINES);
  _buffered_events.push_back({event, mesh});
}


}
