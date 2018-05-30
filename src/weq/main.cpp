#include <weq/ecs/EventManager.hpp>
#include <weq/ecs/EntityManager.hpp>
#include <weq/ecs/SystemManager.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Wave.hpp>
#include <weq/component/Camera.hpp>
#include <weq/primitive/Plane.hpp>
#include <weq/primitive/Sphere.hpp>
#include <weq/primitive/Cube.hpp>
#include <weq/event/Internal.hpp>
#include <weq/event/DebugDraw.hpp>
#include <weq/event/Hotloader.hpp>

#include <weq/EmbeddedData.hpp>

#include <weq/Application.hpp>
#include <weq/system/Hotloader.hpp>
#include <weq/system/Input.hpp>
#include <weq/system/UserInterface.hpp>
#include <weq/system/WaveGPUSimulation.hpp>
#include <weq/system/Camera.hpp>
#include <weq/system/Renderer.hpp>
#include <weq/system/DebugDraw.hpp>

#include <weq/vars/Vars.hpp>
//#include <weq/Hotloader.hpp>

// TODO ActiveWindow skickas ut i början av run(), move.
// TODO window is currently created in application.cpp, move this!
/* TODO
   I should implement sub-systems (for mt)*/
/* TODO
 * Reimplement GLFW->IMGUI bindings so they're implemented more nicely
 */
// TODO config files

class Simulation : public weq::Application{
public:
  Simulation(int argc, char** argv)
    : Application(argc, argv){

    _systems->add<weq::system::Hotloader,         0>()->set_debug_name("hotloader");
    _systems->add<weq::system::Input,             1>()->set_debug_name("input");
    _systems->add<weq::system::UserInterface,     2>()->set_debug_name("ui");
    //_systems->add<weq::system::WaveGPUSimulation, 3>()->set_debug_name("wave gpu");
    _systems->add<weq::system::Camera,            4>()->set_debug_name("camera");
    //_systems->add<weq::system::DebugDraw,         5>()->set_debug_name("debug draw");
    _systems->add<weq::system::Renderer,          6>()->set_debug_name("renderer");

    _systems->configure(*_events);

    // Init vars
    weq::resource_manager::load_tweak_file("System.vars");
    //weq::vars::read_file("..\\res\\System.vars");
    //weq::hotloader::add_directory("..\\res");
    //weq::hotloader::add("..\\res\\System.vars", [](auto path){weq::vars::read_file(path);});

    //_events->emit(weq::event::Track{"..\\res\\System.vars", [](auto path){
    //      weq::vars::read_file(path);
    //    }});
  }

  void configure() override{
    // Draw axes

    // Configure stuff
    configure_states();
    add_camera();
    add_wave();
    add_ui();
    add_reflection_sphere();
    add_reflection_cube();
  }

  void configure_states(){
    // Not implemented!:):):):)
    //  _state_manager.add({"default", {
    //          {},
    //          {},
    //          {}
    //        }});
    //  _state_manager.add({"simulation", {
    //        {},
    //        {},
    //        {}
    //      }});
  }

  void add_reflection_sphere(){
    auto shader = weq::memory::resource_manager::load_shader_program("reflection.prog");
    auto mesh = std::make_shared<weq::Mesh>(weq::primitive::sphere::uv(1.0f, 100, 100), weq::gl::DrawMode::TRIANGLES);

    auto e = _entities->create();
    auto t = _entities->assign<weq::component::Transform>(e)->_position = {2.0f, 0, 2.0f};
    (void)t;
	  auto r = _entities->assign<weq::component::Renderable>(e, mesh);
	  r->scene = shader;
    r->require_camera_pos = true;
    r->require_skybox     = true;
    //r->wireframe          = true;
  }

  void add_reflection_cube(){
    auto shader = weq::memory::resource_manager::load_shader_program("reflection.prog");
    auto mesh = std::make_shared<weq::Mesh>(weq::primitive::cube::solid(1.0f), weq::gl::DrawMode::TRIANGLES);

    auto e = _entities->create();
    auto t = _entities->assign<weq::component::Transform>(e)->_position = {-2.0f, 0, 2.0f};
    (void)t;
	  auto r = _entities->assign<weq::component::Renderable>(e, mesh);
	  r->scene = shader;
    r->require_camera_pos = true;
    r->require_skybox     = true;
    //r->wireframe          = true;
  }

  void add_camera(){
    using namespace weq::component;
    auto c = _entities->create();
    _entities->assign<Camera>(c, LookMode::TARGET);
    _entities->assign<Transform>(c)->spherical(10, 0, 0);
    _entities->assign<ActiveCamera>(c);
  }

  void add_wave(){
    float resolution = 100.0f;
    float size = 5.0f;

    // Scene shader


    //auto wave_mesh = std::make_shared<Mesh>(gl::DrawMode::LINES);
    auto wave_mesh = std::make_shared<weq::Mesh>(weq::gl::DrawMode::TRIANGLES);

    //wave_mesh->generate_vao(scene_p);
    // Mesh for wave plane
    // TODO This operation takes a really long time, which doesn't make sense.
    //auto wave_mesh_data = primitive::plane::solid(resolution,
    //                                              resolution,
    //                                              size/resolution,
    //                                              gl::format::VNCT);

    //wave_mesh->set_data(wave_mesh_data);
    // TODO this code is currently generating an error, because unused shader attributes is being optimzed away => glGetAttribLocation == -1

    using namespace weq::component;
    auto wave = _entities->create();
    auto wave_gpu = _entities->assign<WaveGPU>(wave,
                                               resolution,
                                               resolution,
                                               size/resolution,
                                               0.2f);

    wave_gpu->vel_fbo.texture()->set_parameters({
        {GL_TEXTURE_BASE_LEVEL, 0},
        {GL_TEXTURE_MAX_LEVEL, 0},

        {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
        {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},

        {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
      });
    wave_gpu->height_fbo.texture()->set_parameters({
        {GL_TEXTURE_BASE_LEVEL, 0},
        {GL_TEXTURE_MAX_LEVEL, 0},

        {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
        {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},

        {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
        });
    wave_gpu->edge_fbo.texture()->set_parameters({
        {GL_TEXTURE_BASE_LEVEL, 0},
        {GL_TEXTURE_MAX_LEVEL, 0},

        {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
        {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},

        {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
        {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
        });


    _entities->assign<Transform>(wave)->_position = {-size/2, -size/2, 0};
    _entities->assign<Renderable>(wave, wave_mesh);
  }

  //TODO improve UI
  void add_ui(){
    auto ui = _entities->create();
    auto func = [this](weq::EventManager& e){
      //ImGui::ShowTestWindow();

      ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::SetWindowCollapsed("Debug", true, ImGuiSetCond_FirstUseEver);
      ImGui::Text(_entities->debug_info().c_str());
      ImGui::Text("Engine update frequency: %.1f/%.1f", _current_update_frequency, 1.0/_timestep_value);
      ImGui::Text(_systems->debug_info().c_str());
      ImGui::End();

    };

    _entities->assign<weq::component::ImGui>(ui, func);
  }

  void update(double dt) override{
    //weq::hotloader::update();
    _systems->update_all(*_entities, *_events, dt);
  }
};

int main(int argc, char** argv){
  Simulation sim(argc, argv);
  sim.configure();
  sim.run();

  return 0;
}
