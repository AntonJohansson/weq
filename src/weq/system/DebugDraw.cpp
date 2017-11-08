#include <weq/system/DebugDraw.hpp>
#include <weq/event/DebugDraw.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/Mesh.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/primitive/Vector.hpp>

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
  auto passthrough_v = std::make_shared<gl::Shader>("vertex", gl::ShaderType::VERTEX, vertex_source);
  auto passthrough_f = std::make_shared<gl::Shader>("fragment", gl::ShaderType::FRAGMENT, fragment_source);
  passthrough_v->load();
  passthrough_f->load();

  _shader = std::make_shared<gl::ShaderProgram>("passthrough", passthrough_v, passthrough_f);
  _shader->load();
  _shader->link();
}

void DebugDraw::update(ex::EntityManager& entities,
            ex::EventManager& events,
            ex::TimeDelta dt){
  for(auto& event : _buffered_events){
    auto mesh = std::make_shared<Mesh>(primitive::vector::solid(event.vec, {1, 0, 0, 0}), gl::DrawMode::LINES);
    _meshes.push_back(mesh);
    auto e = entities.create();
    e.assign<component::Transform>()->_translate = event.pos;
    e.assign<component::Renderable>(mesh)->scene = _shader;
  }
}

void DebugDraw::receive(const event::DebugDraw& event){
  _buffered_events.push_back(event);
}

}
