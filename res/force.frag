#version 410

in vec3 v_normal;
in vec4 v_color;
in vec2 v_texcoord;

out vec4 frag_color;

//uniform sampler2D height_field;
uniform vec2 gridsize;
uniform float r;

void main(){
//  vec4 w = texture(height_field, vec2(v_texcoord.x - gridsize.x, v_texcoord.y));
//  vec4 e = texture(height_field, vec2(v_texcoord.x + gridsize.x, v_texcoord.y));
//  vec4 n = texture(height_field, vec2(v_texcoord.x, v_texcoord.y + gridsize.y));
//  vec4 s = texture(height_field, vec2(v_texcoord.x, v_texcoord.y - gridsize.y));
//  vec4 t = texture(height_field, v_texcoord);
//
//  vec4 f = r*(w + e + n + s - 4.0*t);

  frag_color = vec4(1.0, 0.0, 0.0, 1.0);
}
