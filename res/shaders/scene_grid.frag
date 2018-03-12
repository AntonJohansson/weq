#version 410

in vec3 v_normal;
in vec4 v_color;
in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D r_field;
uniform float k; // converts 1/r -> n^2

void main(){
//  float r = texture(r_field, v_texcoord).r;
//
//  vec4 color = vec4(0, 0, 0.25, 1);
//  if(r != 0){
//    color += 0.25*k*(1.0/r);
//  }

  vec4 color = 0.25*texture(r_field, v_texcoord).r + vec4(0,0,0.25,1);

  frag_color = color;
}
