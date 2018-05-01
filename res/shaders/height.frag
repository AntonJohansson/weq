#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D vel_field;
uniform float dt;

// TEMPS
uniform float t;
uniform float c;
uniform float wavelength;
uniform int   wall_item;
uniform float wall_pos;
uniform float slit_sep;
uniform float slit_width;
uniform float amplitude;
uniform vec2 pixelsize;

bool aabb(float l, float r, float u, float d){
  if(v_texcoord.x > l - pixelsize.x/2 && v_texcoord.x < r + pixelsize.x/2 &&
     v_texcoord.y > d - pixelsize.y/2 && v_texcoord.y < u + pixelsize.y/2){
    return true;
  }
  return false;
}

bool single_slit(){
  if(aabb(0,0.5 - slit_width/2, wall_pos, wall_pos) ||
     aabb(0.5 + slit_width/2, 1, wall_pos, wall_pos)){
    frag_color = vec4(0,0,0,0);
    return true;
  }
  return false;
}

bool double_slit(){
  if(aabb(0,0.5 - slit_width/2 - slit_sep/2, wall_pos, wall_pos)  ||
     aabb(0.5 + slit_width/2 + slit_sep/2, 1, wall_pos, wall_pos) ||
     aabb(0.5 - slit_sep/2, 0.5 + slit_sep/2, wall_pos, wall_pos)){
    frag_color = vec4(0,0,0,0);
    return true;
  }
  return false;
}

bool single_slit_2(){
  if(aabb(0,0.5 - slit_width/2, wall_pos, wall_pos)  ||
     aabb(0.5 + slit_width/2, 1, wall_pos, wall_pos) ||
     aabb(0,0.5 - slit_width/2, 1-wall_pos, 1-wall_pos)  ||
     aabb(0.5 + slit_width/2, 1, 1-wall_pos, 1-wall_pos)){
    frag_color = vec4(0,0,0,0);
    return true;
  }
  return false;
}

float pi        = 3.1415;

bool sine_wave(){
  if(gl_FragCoord.y <= 1){
    frag_color = vec4(amplitude*sin(2*pi*(c/wavelength)*t), 0,0,0);
    return true;
  }
  return false;
}

void main(){
  float h = texture(height_field, v_texcoord).r;
  h = texture(height_field, v_texcoord).r + texture(vel_field, v_texcoord).r * dt;

  if(wall_item == 1 && (single_slit() || sine_wave())){}
  else if(wall_item == 2 && (double_slit() || sine_wave())){}
  else{
    frag_color = vec4(h, 0,0,0);
  }
}
