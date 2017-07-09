#include <weq/Engine.hpp>
#include <weq/Window.hpp>
#include <weq/Camera.hpp>
#include <weq/gl/VertexFormat.hpp>
#include <weq/gl/VertexArray.hpp>
#include <weq/gl/Shader.hpp>
#include <weq/Node.hpp>

#include <glad/glad.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <spdlog/spdlog.h>

#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <set>

namespace engine{
namespace{
  constexpr int k_width = 640;
  constexpr int k_height = 480;
  auto _console = spdlog::stderr_color_mt("console");

  std::set<Node*> _nodes; // hanterar noder på helt fel sätt, ska bara ha en rotnod
//  sf::RenderWindow window(sf::VideoMode(k_width, k_height),
//                          "WaveEquationFDM",
//                          sf::Style::Default,
//                          sf::ContextSettings(
//                            24, // depthbits
//                            8, // stencilbits
//                            2, // antialiasing
//                            3, // major version
//                            3,  // minor version
//                            sf::ContextSettings::Core
//                            ));
  Window window;
}

void event_loop(){
  Window::poll_events();
  //sf::Event event;
  //while(window.pollEvent(event)){
  //  if(event.type == sf::Event::Closed){
  //    window.close();
  //  }
  //}
}

void update(float delta_time){
  camera::update();

  for(auto* node : _nodes){
    node->update(delta_time);
  }
}

void render(float delta_time){
  (void)delta_time;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  for(auto* node : _nodes){
    node->_shader->use();
    node->_shader->set("model", node->_model);
    node->_shader->set("view", camera::view());
    node->_shader->set("projection", camera::projection());
    node->_shader->set("normal_matrix", camera::normal_matrix());

    for(const auto& vao : node->_vaos){
      vao.bind();
      //glDrawArrays(GL_POINTS, 0, vao.size());
      node->_ebo.bind();

      glDrawElements(GL_TRIANGLES, node->_ebo.size(), GL_UNSIGNED_INT, 0);
    }
  }

  window.swap_buffers();
}

void add_node(Node* node){
  _nodes.insert(node);
}

void main_loop(){
  using namespace std::chrono_literals;
  using Clock = std::chrono::high_resolution_clock;

  constexpr std::chrono::nanoseconds timestep(16ms);
  std::chrono::nanoseconds lag(0ns);

  auto start_time = Clock::now();
  while(!window.should_close()){
    auto new_time = Clock::now();
    auto delta_time = new_time - start_time;
    start_time = new_time;

    lag += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time);
    while(lag >= timestep){
      lag -= timestep;

      event_loop();
      update(std::chrono::duration_cast<std::chrono::duration<float>>(timestep).count());
      render(std::chrono::duration_cast<std::chrono::duration<float>>(timestep).count());
    }
  }
}

void initialize(){
  if(!gladLoadGL()){std::cerr << "fuck" << std::endl;}
  //spdlog::get("console")->debug();
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  camera::set_aspect(k_width/k_height);
  camera::set_fov(45.0f);
  camera::set_pos({0, 0, 10});
  camera::set_dir({0, 0, -1});
  camera::calculate_perspective();
}

float width(){return (float)k_width;}
float height(){return (float)k_height;}

};
