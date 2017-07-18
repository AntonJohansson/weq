#version 410

in vec3 position;
in vec3 normal;
in vec4 color;
in vec2 texcoord;

out vec3 V;
out vec3 N;
out vec4 Color;
out vec2 Texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal_matrix;

void main(){
  gl_Position = projection * view * model * vec4(position, 1.0);

  V = vec3(projection*view*model*vec4(position, 1.0));
  //V = vec3(view * vec4(position, 1.0));
  //N = vec3(normalize(normal_matrix * vec4(normal, 1.0)));
  N = normal;

  Color = color;
  Texcoord = texcoord;
  //gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  //gl_FrontColor = gl_Color;
}
