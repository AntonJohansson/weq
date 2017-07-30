#version 410

in vec3 Normal;
in vec4 Color;
in vec2 Texcoord;

out vec4 frag_color;

uniform sampler2D tex;
//uniform sampler3D cubemap;

void main(){
  frag_color = Color;
}
