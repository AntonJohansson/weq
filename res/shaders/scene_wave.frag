#version 410

//in vec3 v_normal;
//in vec4 v_color;
in vec2 v_texcoord;
in float v_height;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D normal_field;

in vec3 v_position;
uniform samplerCube skybox;
uniform vec3 camera_pos;
uniform mat4 model; // which model lol...
uniform float reflectivity;

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

  vec3 normal_color = texture(normal_field, v_texcoord).rgb;
  // texture2D -> texure (on mac atleast)
  //vec3 normal_color = texture2D(normal_field, v_texcoord).rgb;
  vec3 normal = (2*normal_color) - vec3(1,1,1);

  normal = mat3(transpose(inverse(model))) * normal;

  vec3 I = normalize(v_position - camera_pos);
  vec3 R = reflect(I, normalize(normal));

  // since I'm using the notation of +z = up, I have to flip the axes
  // to get the ray to line up! :) anoying...
  R = vec3(R.x, R.z, -R.y);

  frag_color = vec4(0.1, 0.1, 0.1, 1) + reflectivity*vec4(texture(skybox, R).rgb, 1.0) + color;
  //frag_color = vec4(0,0,0,0);
//
//
//  color = vec4(normal, 1);
//  frag_color = color;
  //frag_color = vec4(1,1,1,1);
}
