#version 410

in vec3 V;
in vec3 N;
in vec4 Color;
in vec2 Texcoord;

out vec4 frag_color;

uniform sampler2D tex;

void main(){
//  vec3 L = normalize(lightpos - v);
//  vec4 diff = someproduct*max(dot(N,L), 0.0);
//  diff = clamp(diff, 0.0, 1.0);

  //frag_color = vec4(N, 1.0);
  //frag_color = vec4(red, green, blue, 1.0);
  //frag_color = Color;
  frag_color = texture(tex, Texcoord) * Color;
}
