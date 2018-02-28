#version 410

in vec3 v_texcoords;

out vec4 frag_color;

uniform samplerCube cube_texture;

void main(){
  frag_color = texture(cube_texture, v_texcoords);
} 