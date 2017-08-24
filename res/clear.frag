#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D field;

void main(){
  frag_color = vec4(0,0,0,0);
}
