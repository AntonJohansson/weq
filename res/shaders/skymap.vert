#version 410

in vec3 position;

out vec3 v_texcoords;

uniform mat4 vp;

void main(){
  v_texcoords = position;
  //gl_Position = vp * vec4(position, 1.0);
  gl_Position = vec4(position, 1.0);
}
