#include <weq/gl/Shader.hpp>
#include <weq/gl/ShaderProgram.hpp>
#include <weq/component/Renderable.hpp>
#include <weq/component/Transform.hpp>
#include <weq/component/ImGui.hpp>
#include <weq/component/Wave.hpp>
#include <weq/component/Camera.hpp>
#include <weq/primitive/Plane.hpp>
#include <weq/event/Internal.hpp>

#include <weq/Application.hpp>
#include <weq/system/Input.hpp>
#include <weq/system/WaveSimulation.hpp>
#include <weq/system/Camera.hpp>
#include <weq/system/Renderer.hpp>

/* TODO
   I should implement sub-systems (for mt)*/
/* TODO
   Reimplement GLFW->IMGUI bindings so they're implemented more nicely*/

class Simulation : public weq::Application{
public:
  Simulation()
    : Application(){
    _systems.add<weq::system::Input>();

    _systems.add<weq::system::WaveSimulation>();

    _systems.add<weq::system::Camera>();
    _systems.add<weq::system::Renderer>();
    _systems.configure();
  }

  void configure() override{
    add_camera();
    add_wave();
    add_ui();
  }

  void add_camera(){
    auto c = _entities.create();
    c.assign<component::Camera>(LookMode::TARGET);
    c.assign<component::Transform>();
    //TODO something's fishy
    //c.component<component::Transform>()->_translate = c.component<component::Camera>()->position;
    c.assign<component::ActiveCamera>();
  }

  void add_wave(){
    float resolution = 150.0f;
    float size = 5.0f;

    auto v = _resources.get<gl::Shader>("vertex.vert");
    auto f = _resources.get<gl::Shader>("fragment.frag");
    auto p = _resources.get<gl::ShaderProgram>("default.prog", v, f);

    auto mesh_data = primitive::plane::solid(resolution, resolution, size/resolution);
    auto mesh = new Mesh(mesh_data, p);

    auto wave = _entities.create();
    wave.assign<component::Wave>(resolution, resolution, size/resolution, 0.5f);
    wave.assign<component::Transform>()->_translate = {-size/2, -size/2, 0};
    wave.assign<component::Renderable>(mesh, p);
  }

  void add_ui(){
    auto ui = _entities.create();
    ui.assign<component::ImGui>([](ex::EventManager& e){
        ImGui::Begin("Menu");
        ImGui::SetWindowCollapsed("Menu", true, ImGuiSetCond_FirstUseEver);
        ImGui::SetWindowPos("Menu", ImVec2(0,0), ImGuiSetCond_Appearing);

        static float f = 0.0f;
        static bool show_debug = false;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        if(ImGui::Button("Debug"))show_debug ^= 1;

        if(ImGui::Button("Exit")){
          e.emit(event::Quit());
        }

        ImGui::End();

        if(show_debug){
          ImGui::SetNextWindowPos(ImVec2(200,0), ImGuiSetCond_Appearing);
          ImGui::Begin("Debug Window", &show_debug);
          ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
          ImGui::End();
        }
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
