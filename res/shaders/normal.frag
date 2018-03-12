#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform vec2 pixelsize;

void main(){
  vec2 uv = v_texcoord;

  float base_height = texture(height_field, uv).r;

  vec3 average = vec3(0,0,0);

  vec3 l = vec3(0,0,0);
  vec3 r = vec3(0,0,0);
  vec3 d = vec3(0,0,0);
  vec3 u = vec3(0,0,0);

  // left
  if(uv.x - pixelsize.x > 0){
    l = vec3(-1, 0, 0);
    l.z = textureOffset(height_field, uv, ivec2(-1,0)).r - base_height;
  }
  // right
  if(uv.x + pixelsize.x < 1){
    r = vec3(1, 0, 0);
    r.z = textureOffset(height_field, uv, ivec2(1,0)).r - base_height;
  }
  // down
  if(uv.y + pixelsize.x < 1){
    d = vec3(0, 1, 0);
    d.z = textureOffset(height_field, uv, ivec2(0,1)).r - base_height;
  }
  // up
  if(uv.y - pixelsize.x > 0){
    u = vec3(0, -1, 0);
    u.z = textureOffset(height_field, uv, ivec2(0,-1)).r - base_height;
  }

  average = cross(r, u) + cross(u, l) + cross(l, d) + cross(d, r);

  average = -normalize(average);

  frag_color = vec4((average + vec3(1,1,1))/2, 1);
}
