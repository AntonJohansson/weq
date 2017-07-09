#include <weq/Window.hpp>
#include <weq/Engine.hpp>
#include <weq/Node.hpp>
#include <weq/gl/VertexArray.hpp>
#include <weq/FlatBuffer.hpp>
#include <weq/Camera.hpp>

#include <sfml/Window/Mouse.hpp>
#include <sfml/Window/Keyboard.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <deque>
#include <algorithm>
#include <cmath>
#include <iostream>

class Grid : public Node{
public:
  Grid(int w, int h, float gridsize)
    : _width(w),
      _height(h),
      _gridsize(gridsize),
      _height_field(w+1, h+1),
      _delta(w+1, h+1){
    _model = glm::translate(_model, {-w*gridsize/2.0f, -h*gridsize/2.0f, 0.0f});

    _shader = new gl::Shader("vertex.vert", "fragment.frag");

    for(int x = 0; x <= _width; x++){
      for(int y = 0; y <= _height; y++){
        vertices.push_back(x * _gridsize); // x
        vertices.push_back(y * _gridsize); // y
        vertices.push_back(0.0f);          // z

        colors.push_back(1.0f);
        colors.push_back(1.0f);
        colors.push_back(1.0f);
        colors.push_back(1.0f);

        if(y < _height && x < _width){
          elements.push_back(x*(_width+1) + y);        // bottom left
          elements.push_back(x*(_width+1) + (y+1));    // top left
          elements.push_back((x+1)*(_width+1) + (y+1));// top right

          elements.push_back((x+1)*(_width+1) + (y+1));// top right
          elements.push_back((x+1)*(_width+1) + y);    // bottom right
          elements.push_back(x*(_width+1) + y);        // bottom left
        }
      }
    }

    _vertex = {gl::Usage::STREAM, vertices};
    _color = {gl::Usage::STATIC, colors};

    _vaos.push_back({_shader, {
          {_vertex, gl::formats::V},
          {_color, gl::formats::C}
        }});

    _ebo = {gl::Usage::STATIC, elements};

    _mapped_vertices = _vertex.map(gl::Access::READ_WRITE);
    _mapped_colors = _color.map(gl::Access::READ_WRITE);
    _delta.fill(0);
    _height_field.fill(0);

    for(int j = 0; j <= _height; j++){
      _height_field(_width/2, j) = 3.0f;
    }
    _c = 0.2f;
    //_height_field(_width/2, _height/2) = 5.0f;
  }

  ~Grid(){
    _vertex.unmap();
    _color.unmap();
  }

  void color(int i, int j, float r, float g, float b){
    _mapped_colors[(i*(_width + 1) + j)*4 + 0] = 1.0f - r;
    _mapped_colors[(i*(_width + 1) + j)*4 + 1] = 0;
    _mapped_colors[(i*(_width + 1) + j)*4 + 2] = 0;
  }

  float& region_z(int i, int j, int offset = 2){
    assert((i*(_width + 1) + j)*3 + offset < vertices.size());
    return _mapped_vertices[(i*(_width + 1) + j) * 3 + offset];
  }

  void muller(float dt){}

  void iwave(float dt){}

  void ewave(float dt){}

  void update(float dt) override {
    static float gridsize2 = _gridsize*_gridsize;
    static float r = _c*_c*dt*dt/gridsize2;
    static float c2 = _c*_c;

    for(int i = 0; i <= _width; i++){
      for(int j = 0; j <= _height; j++){
        auto l = _height_field(std::max(i-1, 0), j);
        auto r = _height_field(std::min(i+1, (int)_width), j);
        auto n = _height_field(i, std::min(j+1, (int)_height));
        auto s = _height_field(i, std::max(j-1, 0));

        float f = c2*(l + r + n + s - 4.0f*_height_field(i,j))/gridsize2;
        _delta(i,j) += f*dt;
        _delta(i,j) *= 0.997f;

        region_z(i,j) = _height_field(i,j) + _delta(i,j)*dt;
        //float col = region_z(i,j);
        //color(i,j, col, col, col);
      }
    }

    for(int i = 0; i <= _width; i++){
      for(int j = 0; j <= _height; j++){
        _height_field(i,j) = region_z(i,j);
      }
    }
  }

private:
  float _c;
  std::deque<FlatBuffer<float>> _history;
  std::vector<float> vertices, colors;
  std::vector<unsigned int> elements;
  float* _mapped_colors;
  float* _mapped_vertices;
  FlatBuffer<float> _height_field;
  FlatBuffer<float> _delta;
  unsigned int _width;
  unsigned int _height;
  float _gridsize;
  gl::VertexBuffer _vertex;
  gl::VertexBuffer _color;
};

int main(){
  engine::initialize();

  auto size = 100.0f;
  Grid grid(size, size, 5.0f/size);
  engine::add_node(&grid);

  engine::main_loop();

  return 0;
}
