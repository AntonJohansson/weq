#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D framebuffer;

void main(){
  frag_color = texture(framebuffer, v_texcoord);
}
