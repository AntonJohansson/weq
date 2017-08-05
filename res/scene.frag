#version 410

in vec3 v_normal;
in vec4 v_color;
in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D tex;
//uniform sampler3D cubemap;

void main(){
  frag_color = v_color;
}
