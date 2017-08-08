#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D vel_field;
uniform float dt;

void main(){
  frag_color = texture(height_field, v_texcoord) +
               texture(vel_field, v_texcoord) * dt;
}
