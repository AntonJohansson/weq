#include <weq/Engine.hpp>
#include <weq/Node.hpp>
#include <weq/gl/VertexArray.hpp>
#include <weq/FlatBuffer.hpp>
#include <weq/Camera.hpp>

#include <sfml/Window/Mouse.hpp>
#include <sfml/Window/Keyboard.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
      _height_field(w, h),
      _delta(w, h){
    _model = glm::translate(_model, {-w*gridsize/2.0f, -h*gridsize/2.0f, 0.0f});

    _shader = new gl::Shader("vertex.vert", "fragment.frag");

    for(int x = 0; x <= _width; x++){
      for(int y = 0; y <= _height; y++){
        vertices.push_back(x * _gridsize); // x
        vertices.push_back(y * _gridsize); // y
        vertices.push_back(0.0f);         // z

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

    _v = {gl::Usage::STREAM, vertices};
    _c = {gl::Usage::STATIC, colors};

    _vaos.push_back({_shader, {
          {_v, gl::formats::V},
          {_c, gl::formats::C}
        }});

    _ebo = {gl::Usage::STATIC, elements};

    _mapped_region = _v.map(gl::Access::READ_WRITE);
    _delta.fill(0);
    _height_field.fill(0);

    //for(int j = 0; j <= _height; j++){
    //  region_z(2, j) = 0.25f;
    //}
    _history.push_front(_height_field);
    _history.push_front(_height_field);
    //region_z(0, _height/2) = 0.5f;
  }


  ~Grid(){
    _v.unmap();
  }

  float& region_z(int i, int j, int offset = 2){
    return _mapped_region[(i*(_width + 1) + j) * 3 + offset];
  }

  void update(float dt) override {
    static float gridsize2 = _gridsize*_gridsize;
    static float c = 3.0f;
    static float r = c*c*dt*dt/gridsize2;

    for(int i = 0; i <= _width; i++){
      for(int j = 0; j <= _height; j++){
        auto l = _height_field(std::max(i-1, 0), j);
        auto r = _height_field(std::min(i+1, (int)_width), j);
        auto n = _height_field(i, std::min(j+1, (int)_height));
        auto s = _height_field(i, std::max(j-1, 0));

        _delta(i,j) = (l + r + n + s - 4.0f*_height_field(i,j));
      }
    }

    for(int i = 0; i <= _width; i++){
      for(int j = 0; j <= _height; j++){
        region_z(i,j) = 0.5f*(+ r*_delta(i,j)
                              + 5*_height_field(i,j)
                              - 4*_history[0](i,j)
                              + 1*_history[1](i,j));

      }
    }

    for(int i = 0; i <= _width; i++){
      for(int j = 0; j <= _height; j++){
        _height_field(i,j) = region_z(i,j);
      }
    }

    _history.push_front(_height_field);
    _history.pop_back();
  }

private:
  std::deque<FlatBuffer<float>> _history;
  std::vector<float> vertices, colors;
  std::vector<unsigned int> elements;
  float* _mapped_region;
  FlatBuffer<float> _height_field;
  FlatBuffer<float> _delta;
  unsigned int _width;
  unsigned int _height;
  float _gridsize;
  gl::VertexBuffer _v;
  gl::VertexBuffer _c;
};

int main(){
  engine::intialize();

  auto size = 100.0f;
  Grid grid(size, size, 5.0f/size);
  engine::add_node(&grid);

  engine::main_loop();

  return 0;
}
