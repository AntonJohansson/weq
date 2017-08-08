#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;

float gaussian(float A, vec2 p, vec2 p0, vec2 sigma){
  return exp(-0.5*(pow((p.x - p0.x)/sigma.x, 2) +
                   pow((p.y - p0.y)/sigma.y, 2)));
}

void main(){
  vec4 color = texture(height_field, v_texcoord);
  float drop = gaussian(0.1, v_texcoord, vec2(0.5, 0.5), vec2(0.1, 0.1));
  frag_color = vec4(drop, drop, 1.0, 1);
}
