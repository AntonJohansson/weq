#version 330

in vec3 V;
in vec3 N;
in vec4 Color;

out vec4 frag_color;

void main(){
//  vec3 L = normalize(lightpos - v);
//  vec4 diff = someproduct*max(dot(N,L), 0.0);
//  diff = clamp(diff, 0.0, 1.0);

  frag_color = Color;
}
