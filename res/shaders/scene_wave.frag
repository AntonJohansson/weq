#version 410

//in vec3 v_normal;
//in vec4 v_color;
in vec2 v_texcoord;
in float v_height;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D normal_field;

void main(){
  float h = v_height;

  // Change color depending on height
  vec4 color = vec4(0,0,0,0);

  if(h > 0){
    color += vec4(h, 0, 0.25, 1);
  }else if (h == 0){
    color += vec4(0, 0, 0.25, 1);
  }else{
    color += vec4(0, -h, 0.25, 1);
  }

  vec3 normal_color = texture2D(normal_field, v_texcoord).rgb;
  vec3 normal = (2*normal_color) - vec3(1,1,1);
  color = vec4(normal, 1);
  frag_color = color;
}
