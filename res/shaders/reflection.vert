#version 410

in vec3 position;
in vec3 normal;

out vec3 v_normal;
out vec3 v_position;

uniform mat4 mvp;
uniform mat4 normal_mat;
uniform mat4 camera_model;
uniform mat4 model;

void main(){
  gl_Position = mvp * vec4(position, 1.0);
  v_position = vec3(model * vec4(position, 1.0));
  v_normal = mat3(transpose(inverse(model))) * normal;
  //v_normal = mat3(normal_mat)*normal;
  //v_normal = normal;
}
