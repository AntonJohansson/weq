#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D vel_field; // TEMPORARY - addition can be performed by OpenGL blending.
uniform vec2 gridsize;
uniform float r;
uniform float dt;

void main(){
  float w = texture(height_field, vec2(v_texcoord.x - gridsize.x,
                                      v_texcoord.y)).r;
  float e = texture(height_field, vec2(v_texcoord.x + gridsize.x,
                                      v_texcoord.y)).r;
  float n = texture(height_field, vec2(v_texcoord.x,
                                       v_texcoord.y + gridsize.y)).r;
  float s = texture(height_field, vec2(v_texcoord.x,
                                       v_texcoord.y - gridsize.y)).r;
  float t = texture(height_field, v_texcoord).r;

  float f = r*(w + e + n + s - 4.0*t);

  float dv = f*dt;
  float v = texture(vel_field, v_texcoord).r + dv;

  frag_color = vec4(v, 0, 0 ,0); // Blend mode will add to velocity texture.

  // Even if we read and write to the same texture, changes will not be
  // written after the entire draw call is complete -> no need for
  // double buffering (yay).
}