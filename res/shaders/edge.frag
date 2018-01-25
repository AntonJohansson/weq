#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D edge_field;
uniform vec2 gridsize;
uniform vec2 pixelsize;
uniform float c;
uniform float dt;

void main(){
  float y_value = floor(gl_FragCoord.y);
  float x_value = floor(gl_FragCoord.x);

  float g = texture(edge_field, v_texcoord).r;
  float u, g_new;

  if(y_value == 0){
    u = texture(height_field, vec2(0, v_texcoord.x)).r;
  }else if(y_value == 1){
    u = texture(height_field, vec2(1, v_texcoord.x)).r;
  }else if(y_value == 2){
    u = texture(height_field, vec2(v_texcoord.x, 0)).r;
  }else if(y_value == 3){
    u = texture(height_field, vec2(v_texcoord.x, 1)).r;
  }

  g_new = (c*dt*u + g*gridsize.x)/(gridsize.x + c*dt);
  frag_color = vec4(g_new, 0, 0, 0);
}
