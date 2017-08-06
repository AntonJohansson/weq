#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Wave.hpp>
#include <weq/component/Camera.hpp>
#include <weq/primitive/Plane.hpp>
#include <weq/event/Internal.hpp>

#include <weq/EmbeddedData.hpp>

#include <weq/Application.hpp>
#include <weq/system/Input.hpp>
#include <weq/system/WaveGPUSimulation.hpp>
#include <weq/system/Camera.hpp>
#include <weq/system/Renderer.hpp>

/* TODO
   I should implement sub-systems (for mt)*/
/* TODO
 * Reimplement GLFW->IMGUI bindings so they're implemented more nicely
 */

class Simulation : public weq::Application{
public:
  Simulation()
    : Application(){
    _systems.add<weq::system::Input>();

    //_systems.add<weq::system::WaveSimulation>();
    //_systems.add<weq::system::WaveGPUSimulation>();

    _systems.add<weq::system::Camera>();
    _systems.add<weq::system::Renderer>();
    _systems.configure();
  }

  void configure() override{
    configure_states();
    add_camera();
    add_wave();
    add_ui();
  }

  void configure_states(){
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
    c.assign<component::Transform>();
    c.assign<component::ActiveCamera>();
  }

  void add_wave(){
    float resolution = 100.0f;
    float size = 5.0f;

    // Scene shader
    auto scene_v = _resource_manager.get<gl::Shader>("scene.vert");
    auto scene_f = _resource_manager.get<gl::Shader>("scene.frag");
    auto scene_p = _resource_manager.get<gl::ShaderProgram>("scene.prog",
                                                            scene_v,
                                                            scene_f);
    scene_p->link();

    // Force calculation shader
    auto force_v = _resource_manager.get<gl::Shader>("force.vert");
    auto force_f = _resource_manager.get<gl::Shader>("force.frag");
    auto force_p = _resource_manager.get<gl::ShaderProgram>("force.prog",
                                                            force_v,
                                                            force_f);
    force_p->link();

    // Mesh for wave plane
    auto wave_mesh_data = primitive::plane::solid(resolution,
                                                  resolution,
                                                  size/resolution,
                                                  gl::format::VNCT);

    auto wave_mesh = std::make_shared<Mesh>(wave_mesh_data, gl::DrawMode::TRIANGLES);
    wave_mesh->generate_vao(scene_p);
    wave_mesh->generate_vao(force_p);

    auto wave = _entities.create();
//    wave.assign<component::WaveGPU>(resolution,
//                                    resolution,
//                                    size/resolution,
//                                    0.2f,
//                                    force_p);

    wave.assign<component::Transform>()->_translate = {-size/2, -size/2, 0};
    auto r = wave.assign<component::Renderable>(wave_mesh);
    r->scene = scene_p;
  }

  //TODO improve UI
  void add_ui(){
    auto ui = _entities.create();
    ui.assign<component::ImGui>([](ex::EventManager& e){
        ImGui::ShowTestWindow();
        ImGui::Begin("Menu");
        ImGui::SetWindowCollapsed("Menu", false, ImGuiSetCond_FirstUseEver);
        ImGui::SetWindowPos("Menu", ImVec2(10,10), ImGuiSetCond_Appearing);
        ImGui::SetWindowSize("Menu", ImVec2(200,500), ImGuiSetCond_Appearing);

        if(ImGui::CollapsingHeader("simulation")){
        }

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

int main(){
  Simulation sim;
  sim.configure();
  sim.run();

  return 0;
}
