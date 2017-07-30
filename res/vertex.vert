#version 410

in vec3 position;
in vec3 normal;
in vec4 color;
in vec2 texcoord;

out vec3 Normal;
out vec4 Color;
out vec2 Texcoord;

uniform mat4 mvp;
uniform mat4 normal_matrix;

void main(){
  gl_Position = mvp * vec4(position, 1.0);
  Normal = normal;
  Color = color;
  Texcoord = texcoord;
  //gl_Position = projection * view * model * vec4(position, 1.0);
}
