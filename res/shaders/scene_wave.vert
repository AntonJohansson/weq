#version 410

in vec3 position;
//in vec3 normal;
//in vec4 color;
in vec2 texcoord;

//out vec3 v_normal;
//out vec4 v_color;
out vec2 v_texcoord;
out float v_height;

uniform mat4 mvp;
uniform mat4 normal_matrix;
uniform sampler2D height_field;

void main(){
  float height = textureLod(height_field, texcoord, 0.0).r;
  gl_Position = mvp * vec4(position.xy, position.z + height, 1.0);

  //v_normal = normal;
  //v_color = color;
  v_texcoord = texcoord;
  v_height = height;
}
