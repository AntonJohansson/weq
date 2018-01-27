#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Wave.hpp>
#include <weq/component/Camera.hpp>
#include <weq/primitive/Plane.hpp>
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

    _systems.configure();

    _hotloader = _systems.add<weq::system::Hotloader>();
    _input     = _systems.add<weq::system::Input>();
    _ui        = _systems.add<weq::system::UserInterface>();
    _wgpu      = _systems.add<weq::system::WaveGPUSimulation>();
    _camera    = _systems.add<weq::system::Camera>();
    _ddraw     = _systems.add<weq::system::DebugDraw>();
    _renderer  = _systems.add<weq::system::Renderer>();

    //_systems.configure(); // order not guarranteed between systems
    _hotloader->configure(_events);
    _input->configure(_events);
    _ui->configure(_events);
    _wgpu->configure(_events);
    _camera->configure(_events);
    _ddraw->configure(_events);
    _renderer->configure(_events);


    // Init vars
    weq::vars::read_file("..\\res\\System.vars");
    //weq::hotloader::add_directory("..\\res");
    //weq::hotloader::add("..\\res\\System.vars", [](auto path){weq::vars::read_file(path);});

    _events.emit(weq::event::Track{"..\\res\\System.vars", [](auto path){
          weq::vars::read_file(path);
        }});
  }

  void configure() override{
    // Draw axes

    // Configure stuff
    configure_states();
    add_camera();
    add_wave();
    add_ui();
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

  void add_camera(){
    auto c = _entities.create();
    c.assign<weq::component::Camera>(LookMode::TARGET);
    c.assign<weq::component::Transform>()->spherical(10, 0, 0);
    c.assign<weq::component::ActiveCamera>();
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

    auto wave = _entities.create();
    auto wave_gpu = wave.assign<weq::component::WaveGPU>(resolution,
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


    wave.assign<weq::component::Transform>()->_position = {-size/2, -size/2, 0};
    auto r = wave.assign<weq::component::Renderable>(wave_mesh);
    //xr->scene = scene_p;
  }

  //TODO improve UI
  void add_ui(){
    auto ui = _entities.create();
    ui.assign<weq::component::ImGui>([this](ex::EventManager& e){
        //ImGui::ShowTestWindow();
        ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowCollapsed("Debug", true, ImGuiSetCond_FirstUseEver);
        ImGui::Text("Update rates:\n  Engine    %.1f/%.1f\n  Hotloader %.1f/%.1f\n  Input     %.1f/%.1f\n  Ui        %.1f/%.1f\n  WaveSim   %.1f/%.1f\n  Camera    %.1f/%.1f\n  DebugDraw %.1f/%.1f\n  Renderer  %.1f/%.1f",
                    _current_update_frequency,   1.0/_timestep_value,
                    _hotloader->get_framerate(), 1.0/_hotloader->get_timestep_value(),
                    _input->get_framerate(),     1.0/_input->get_timestep_value(),
                    _ui->get_framerate(),        1.0/_ui->get_timestep_value(),
                    _wgpu->get_framerate(),      1.0/_wgpu->get_timestep_value(),
                    _camera->get_framerate(),    1.0/_camera->get_timestep_value(),
                    _ddraw->get_framerate(),     1.0/_ddraw->get_timestep_value(),
                    _renderer->get_framerate(),  1.0/_renderer->get_timestep_value());
        ImGui::End();

        ImGui::Begin("Menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowCollapsed("Menu", false, ImGuiSetCond_FirstUseEver);
        ImGui::SetWindowPos("Menu", ImVec2(10,10), ImGuiSetCond_FirstUseEver);
        ImGui::End();
      });
  }

  void update(double dt) override{
    //weq::hotloader::update();
    _systems.update_all(dt);
  }

private:
  std::shared_ptr<weq::system::Hotloader>         _hotloader;
  std::shared_ptr<weq::system::Input>             _input;
  std::shared_ptr<weq::system::UserInterface>     _ui;
  std::shared_ptr<weq::system::WaveGPUSimulation> _wgpu;
  std::shared_ptr<weq::system::Camera>            _camera;
  std::shared_ptr<weq::system::DebugDraw>         _ddraw;
  std::shared_ptr<weq::system::Renderer>          _renderer;
};

int main(int argc, char** argv){
  Simulation sim(argc, argv);
  sim.configure();
  sim.run();

  return 0;
}
