#version 410

//in vec3 v_normal;
//in vec4 v_color;
in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform float gridsize;
uniform float r;

//uniform float t;

float gaussian(float A, vec2 p, vec2 p0, vec2 sigma){
  return exp(-0.5*(pow((p.x - p0.x)/sigma.x, 2) +
                   pow((p.y - p0.y)/sigma.y, 2)));
}

void main(){
  vec4 w = texture(height_field, vec2(v_texcoord.x - gridsize, v_texcoord.y));
  vec4 e = texture(height_field, vec2(v_texcoord.x + gridsize, v_texcoord.y));
  vec4 n = texture(height_field, vec2(v_texcoord.x, v_texcoord.y + gridsize));
  vec4 s = texture(height_field, vec2(v_texcoord.x, v_texcoord.y - gridsize));
  vec4 t = texture(height_field, v_texcoord);

  vec4 f = r*(w + e + n + s - 4.0*t);

  float drop = gaussian(1, gl_FragCoord.xy, vec2(50, 50), vec2(20, 20));

  //frag_color = vec4(drop, drop, drop, 1);
  frag_color = vec4(v_texcoord.x, v_texcoord.y, 0, 1);
}
