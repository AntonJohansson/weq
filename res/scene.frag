#version 410

in vec3 v_normal;
in vec4 v_color;
in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
//uniform sampler3D cubemap;

void main(){
  float h = texture(height_field, v_texcoord).r;

  if(h > 0){
    frag_color = vec4(h, 0, 1, 1);
  }else{
    frag_color = vec4(0, h, 1, 1);
  }
}
