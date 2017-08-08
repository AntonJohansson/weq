#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform vec2 gridsize;
uniform float r;

uniform float dt;

void main(){
  vec4 w = texture(height_field, vec2(v_texcoord.x - gridsize.x,
                                      v_texcoord.y));
  vec4 e = texture(height_field, vec2(v_texcoord.x + gridsize.x,
                                      v_texcoord.y));
  vec4 n = texture(height_field, vec2(v_texcoord.x,
                                      v_texcoord.y + gridsize.y));
  vec4 s = texture(height_field, vec2(v_texcoord.x,
                                      v_texcoord.y - gridsize.y));
  vec4 t = texture(height_field, v_texcoord);

  vec4 f = r*(w + e + n + s - 4.0*t);

  frag_color = vec4(abs(f.r), abs(f.g), abs(f.b), 1);

  // Even if we read and write to the same texture, changes will not be
  // written after the entire draw call is complete -> no need for
  // double buffering (yay).
}
