#pragma once

#include <glm/glm.hpp>

namespace camera{

void translate(glm::vec3 v);
void rotate_deg(float deg, glm::vec3 axis);
void rotate_rad(float rad, glm::vec3 axis);
void calculate_perspective();
void update();

glm::vec4 unproject(glm::mat4 model, glm::vec4 v); // should maybe be moved somewhere

void set_fov(float fov);
void set_aspect(float aspect);
void set_speed(float speed);
void set_up(glm::vec3 up);
void set_pos(glm::vec3 p);
void set_dir(glm::vec3 d);

glm::mat4 view();
glm::mat4 projection();
glm::mat4 normal_matrix();

};
