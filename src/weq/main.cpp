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
    _systems.add<weq::system::WaveGPUSimulation>();

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
    float resolution = 1000.0f;
    float size = 5.0f;

    auto rv = _resource_manager.get<gl::Shader>("render_v", "render.vert");
    auto rf = _resource_manager.get<gl::Shader>("render_f", "render.frag");
    auto rp = _resource_manager.get<gl::ShaderProgram>("render.prog", rv, rf);
    rp->link();

    //    auto fv = _resource_manager.get<gl::Shader>("force_v", "force.vert");
    //    auto ff = _resource_manager.get<gl::Shader>("force_f", "force.frag");
    //    auto fp = _resource_manager.get<gl::ShaderProgram>("force.prog", fv, ff);
    //    fp->link();

    auto mesh_data = primitive::plane::solid(resolution,
                                             resolution,
                                             size/resolution,
                                             gl::format::VNCT);

    auto mesh = new Mesh(mesh_data, gl::DrawMode::TRIANGLES);

    auto wave = _entities.create();
    //wave.assign<component::Wave>(resolution, resolution, size/resolution, 0.5f);
    wave.assign<component::Transform>()->_translate = {-size/2, -size/2, 0};
    wave.assign<component::Renderable>(mesh, rp);
  }

  //TODO improve UI
  void add_ui(){
    auto ui = _entities.create();
    ui.assign<component::ImGui>([](ex::EventManager& e){
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
  sim.main_loop();

  return 0;
}
