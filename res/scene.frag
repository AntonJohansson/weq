#version 410

in vec3 v_normal;
in vec4 v_color;
in vec2 v_texcoord;
in float v_height;

out vec4 frag_color;

//uniform sampler2D height_field;
//uniform sampler3D cubemap;
uniform sampler2D ri_field;

void main(){
  float h = v_height;

  vec4 color = texture(ri_field, v_texcoord) + vec4(0, 0, 0.25, 1);
  // Change color depending on height
  //if(h > 0){
  //  color += vec4(h, 0, 0.25, 1);
  //}else if (h == 0){
  //  color += vec4(0, 0, 0.25, 1);
  //}else{
  //  color += vec4(0, -h, 0.25, 1);
  //}

  frag_color = color;
}
