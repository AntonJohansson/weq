#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D vel_field;
uniform float dt;

void main(){
  float h = texture(height_field, v_texcoord).r + texture(vel_field, v_texcoord).r * dt;
  frag_color = vec4(h, 0, 0, 0);
}
