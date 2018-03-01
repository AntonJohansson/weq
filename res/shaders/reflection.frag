#version 410

in vec3 v_normal;
in vec3 v_position;

out vec4 frag_color;

uniform vec3 camera_pos;
uniform samplerCube skybox;

void main(){
  vec3 I = normalize(v_position - camera_pos);
  vec3 R = reflect(I, normalize(v_normal));

  // since I'm using the notation of +z = up, I have to flip the axes
  // to get the ray to line up! :) anoying...
  R = vec3(R.x, R.z, -R.y);

  /*
    (0,  1,  1)
    (0, -1,  1)

    (0,  2,  1)
    (0, -1, 2)
   */

  frag_color = vec4(0.1, 0.1, 0.1, 1) + 0.25*vec4(texture(skybox, R).rgb, 1.0);
  //frag_color = + vec4(v_normal,1);
}
