#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D vel_field; // TEMPORARY - addition can be performed by OpenGL blending.
uniform sampler2D ri_field;
uniform vec2 pixelsize;
uniform vec2 gridsize;
//uniform float r;
uniform float dt;
uniform float c;

void main(){
  float wave_v = c/texture(ri_field, v_texcoord).r;
  float r = wave_v*wave_v/(gridsize.x*gridsize.x);
  float w = texture(height_field, vec2(v_texcoord.x - pixelsize.x,
                                      v_texcoord.y)).r;
  float e = texture(height_field, vec2(v_texcoord.x + pixelsize.x,
                                      v_texcoord.y)).r;
  float n = texture(height_field, vec2(v_texcoord.x,
                                       v_texcoord.y + pixelsize.y)).r;
  float s = texture(height_field, vec2(v_texcoord.x,
                                       v_texcoord.y - pixelsize.y)).r;
  float t = texture(height_field, v_texcoord).r;

  float k = 1.0;
  float f = k*r*(w + e + n + s - 4.0*t);

  float dv = f*dt;
  float v = texture(vel_field, v_texcoord).r + dv;

  frag_color = vec4(v, 0, 0 ,0); // Blend mode will add to velocity texture.

  // Even if we read and write to the same texture, changes will not be
  // written after the entire draw call is complete -> no need for
  // double buffering (yay).
}
