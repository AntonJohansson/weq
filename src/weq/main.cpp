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

    //_systems.add<weq::system::Input>();
    //_systems.add<weq::system::UserInterface>();
    //_systems.add<weq::system::WaveGPUSimulation>();
    //_systems.add<weq::system::Camera>();
    //_systems.add<weq::system::DebugDraw>();
    //_systems.add<weq::system::Renderer>();

    // Order is backwards on mac for some reason.
    // Should systems be order independant?
    // entityx thinks so.
    _systems.add<weq::system::Renderer>();
    _systems.add<weq::system::DebugDraw>();
    _systems.add<weq::system::Camera>();
    _systems.add<weq::system::WaveGPUSimulation>();
    _systems.add<weq::system::UserInterface>();
    _systems.add<weq::system::Input>();


    _systems.configure();
  }

  void configure() override{
    // Add on top (doesnt work)
    _events.emit(event::DebugDraw(event::DrawType::VECTOR, event::DebugMode::PERSISTENT, {1,0,0}, {0,0,0}, {1, 0, 0, 1})); // X
    _events.emit(event::DebugDraw(event::DrawType::VECTOR, event::DebugMode::PERSISTENT, {0,1,0}, {0,0,0}, {0, 1, 0, 1})); // Y
    _events.emit(event::DebugDraw(event::DrawType::VECTOR, event::DebugMode::PERSISTENT, {0,0,1}, {0,0,0}, {0, 0, 1, 1})); // Z
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
    float resolution = 1000.0f;
    float size = 5.0f;

    // Scene shader
    auto scene_v = _resource_manager.get<gl::Shader>("scene.vert");
    auto scene_f = _resource_manager.get<gl::Shader>("scene.frag");
    auto scene_p = _resource_manager.get<gl::ShaderProgram>("scene.prog",
                                                            scene_v,
                                                            scene_f);
    scene_p->link();


    auto wave_mesh = std::make_shared<Mesh>(gl::DrawMode::LINES);

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
                                                    0.02f);

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


    wave.assign<component::Transform>()->_position = {-size/2, -size/2, 0};
    //auto r = wave.assign<component::Renderable>(wave_mesh);
    //r->scene = scene_p;
  }

  //TODO improve UI
  void add_ui(){
    auto ui = _entities.create();
    ui.assign<component::ImGui>([](ex::EventManager& e){
        //ImGui::ShowTestWindow();
        ImGui::Begin("Menu");
        ImGui::SetWindowCollapsed("Menu", false, ImGuiSetCond_FirstUseEver);
        ImGui::SetWindowPos("Menu", ImVec2(10,10), ImGuiSetCond_FirstUseEver);
        ImGui::SetWindowSize("Menu", ImVec2(200,500), ImGuiSetCond_FirstUseEver);

        if(ImGui::CollapsingHeader("settings")){
        }

        if(ImGui::CollapsingHeader("debug")){
          ImGui::Text("Avg. frametime %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        if(ImGui::Button("Exit")){
          e.emit(event::Quit());
        }

        ImGui::End();
      });
  }

  void update(double dt) override{
    _systems.update_all(dt);
  }
};

int main(int argc, char** argv){
  Simulation sim(argc, argv);
  sim.configure();
  sim.run();

  return 0;
}
