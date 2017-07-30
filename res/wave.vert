#version 410

in vec3 position;
in vec4 color;

out vec4 v_color;

uniform mat4 mvp;
uniform mat4 normal_matrix;

void main(){
  gl_Position = mvp*vec4(position, 1.0);
  v_color = color;
}
