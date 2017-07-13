#include <weq/Camera.hpp>
#include <weq/Engine.hpp> // not nice

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace camera{
namespace{
  float _fov = 90.0f;
  float _aspect = 1.0f;
  float _speed = 1.0f;
  glm::vec3 _up{0, 1, 0};
  glm::vec3 _position{0, 0, 0};
  glm::vec3 _direction{1, 0, 0};
  glm::mat4 _view;
  glm::mat4 _projection;
  glm::mat4 _normal_matrix;
}

void translate(glm::vec3 v){
  _position += _speed * v;
}

void forward(float f){
  _position += _direction * f;
}

void right(float f){
  auto right = glm::cross(_direction, _up);
  _position += right * f;
}

void up(float f){
  _position += _up * f;
}

void rotate_deg(float deg, glm::vec3 axis){
  rotate_rad(glm::radians(deg), axis);
}

void rotate_rad(float rad, glm::vec3 axis){
  _direction = glm::rotate(_direction, rad * _speed, axis);
}

void calculate_perspective(){
  _projection = glm::perspective(glm::radians(_fov), _aspect, 1.0f, 100.0f);
}

void update(){
  // camera movement

  //auto right = glm::cross(_direction, _up);
  //auto l_up = glm::cross(right, _direction);
  //float speed = 0.1f;
  //if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
  //  _position -= right * speed;
  //}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
  //  _position += right * speed;
  //}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
  //  _position += _direction * speed;
  //}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
  //  _position -= _direction * speed;
  //}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){
  //  _position -= _up * speed;
  //}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
  //  _position += _up * speed;
  //}

  // orbital cam

  //static float radius = 10.0f;
  //static glm::vec3 target = {0.0f, 0.0f, 0.0f};
  //static glm::vec2 old_v = {0.0f, 0.0f};
  //static bool is_pressed = false;

  //if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
  //  auto mouse_input = sf::Mouse::getPosition();
  //  glm::vec2 v = {mouse_input.x/640.0f, mouse_input.y/480.0f};

  //  if(!is_pressed){
  //    is_pressed = true;
  //    old_v = v;
  //  }

  //  auto dv = v - old_v;
  //  _position += 50.0f*(-right*dv.x + l_up*dv.y);
  //  _position = glm::normalize(_position) * radius;

  //  old_v = v;
  //}else{
  //  is_pressed = false;
  //}

  //_view = glm::lookAt(_position, target, _up);
  _view = glm::lookAt(_position, _position + _direction, _up);
  _normal_matrix = glm::transpose(glm::inverse(_view)); // should be view*model
}

glm::vec4 unproject(glm::mat4 model, glm::vec4 v){
  // normalize coordinates
  v.x = v.x/weq::engine::width()*2.0f - 1.0f;
  v.y = (weq::engine::height() - v.y)/weq::engine::height()*2.0f - 1.0f;
  v.z = v.z*2.0f - 1.0f;
  v.w = 1.0f;

  auto inv_mvp = glm::inverse(projection()*view()*model);

  auto transformed_v = inv_mvp * v;
  if(transformed_v.w == 0.0f){
    return transformed_v;
  }

  transformed_v.w = 1.0f/transformed_v.w;
  transformed_v.x = transformed_v.x*v.w;
  transformed_v.y = transformed_v.y*v.w;
  transformed_v.z = transformed_v.z*v.w;
  return transformed_v;
}

void set_fov(float fov){_fov = fov;}
void set_aspect(float aspect){_aspect = aspect;}
void set_speed(float speed){_speed = speed;}
void set_up(glm::vec3 up){_up = up;}
void set_pos(glm::vec3 p){_position = p;}
void set_dir(glm::vec3 d){_direction = glm::normalize(d);}

glm::mat4 view(){return _view;}
glm::mat4 projection(){return _projection;}
glm::mat4 normal_matrix(){return _normal_matrix;}

}
