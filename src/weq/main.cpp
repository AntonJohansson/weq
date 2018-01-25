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

#include <weq/EmbeddedData.hpp>

#include <weq/Application.hpp>
#include <weq/system/Input.hpp>
#include <weq/system/UserInterface.hpp>
#include <weq/system/WaveGPUSimulation.hpp>
#include <weq/system/Camera.hpp>
#include <weq/system/Renderer.hpp>
#include <weq/system/DebugDraw.hpp>

#include <weq/vars/Vars.hpp>
#include <weq/Hotloader.hpp>

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

    _input    = _systems.add<weq::system::Input>();
    _ui       = _systems.add<weq::system::UserInterface>();
    _wgpu     = _systems.add<weq::system::WaveGPUSimulation>();
    _camera   = _systems.add<weq::system::Camera>();
    _ddraw    = _systems.add<weq::system::DebugDraw>();
    _renderer = _systems.add<weq::system::Renderer>();

    //_systems.configure(); // order not guarranteed between systems
    _input->configure(_events);
    _ui->configure(_events);
    _wgpu->configure(_events);
    _camera->configure(_events);
    _ddraw->configure(_events);
    _renderer->configure(_events);

    // Init vars
    weq::vars::read_file("..\\res\\System.vars");
    //weq::hotloader::add_directory("..\\res");
    weq::hotloader::add("..\\res\\System.vars", [](auto path){weq::vars::read_file(path);});
    //weq::hotloader::add("..\\res\\shaders", [](auto){});
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
    c.assign<component::Camera>(LookMode::TARGET);
    c.assign<component::Transform>()->spherical(10, 0, 0);
    c.assign<component::ActiveCamera>();
  }

  void add_wave(){
    float resolution = 100.0f;
    float size = 5.0f;

    // Scene shader


    //auto wave_mesh = std::make_shared<Mesh>(gl::DrawMode::LINES);
    auto wave_mesh = std::make_shared<Mesh>(gl::DrawMode::TRIANGLES);

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
    auto wave_gpu = wave.assign<component::WaveGPU>(resolution,
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


    wave.assign<component::Transform>()->_position = {-size/2, -size/2, 0};
    auto r = wave.assign<component::Renderable>(wave_mesh);
    //xr->scene = scene_p;
  }

  //TODO improve UI
  void add_ui(){
    auto ui = _entities.create();
    ui.assign<component::ImGui>([](ex::EventManager& e){
        //ImGui::ShowTestWindow();
        ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowCollapsed("Debug", true, ImGuiSetCond_FirstUseEver);
        //ImGui::SetWindowPos("Debug", ImVec2(10,), ImGuiSetCond_FirstUseEver);
        ImGui::Text("Avg. frametime %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        ImGui::Begin("Menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowCollapsed("Menu", false, ImGuiSetCond_FirstUseEver);
        ImGui::SetWindowPos("Menu", ImVec2(10,10), ImGuiSetCond_FirstUseEver);


        ImGui::End();
      });
  }

  void update(double dt) override{
    weq::hotloader::update();
    _systems.update_all(dt);
  }

private:
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
