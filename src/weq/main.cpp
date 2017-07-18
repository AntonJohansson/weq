#include <weq/Engine.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/Wave.hpp>
#include <weq/primitive/Plane.hpp>
#include <weq/resource/ResourceManager.hpp>

//class Simulation : public weq::Application{
//};

int main(){
  weq::engine::initialize();

  float resolution = 150.0f;
  float size = 5.0f;

  auto res = weq::engine::resource_mgr();

  auto v = res->get<gl::Shader>("vertex.vert");
  auto f = res->get<gl::Shader>("fragment.frag");
  auto p = res->get<gl::ShaderProgram>("default.prog", v, f);

  auto mesh_data = primitive::plane::solid(resolution, resolution, size/resolution);
  auto mesh = new Mesh(mesh_data, p);

  auto wave = weq::engine::create_entity();
  wave.assign<component::Wave>(resolution, resolution, size/resolution, 0.5f);
  wave.assign<component::Transform>()->translate({-size/2, -size/2, 0});
  wave.assign<component::Renderable>(mesh, p);

  weq::engine::main_loop();

  return 0;
}
