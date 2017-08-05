#version 410

in vec3 position;
in vec3 normal;
in vec4 color;
in vec2 texcoord;

out vec3 v_normal;
out vec4 v_color;
out vec2 v_texcoord;

uniform mat4 mvp;
uniform mat4 normal_matrix;

void main(){
  gl_Position = mvp * vec4(position, 1.0);
  v_normal = normal;
  v_color = color;
  v_texcoord = texcoord;
}
