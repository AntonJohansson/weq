#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform vec2 pixelsize;

void main(){
  vec2 uv = v_texcoord;

  vec3 base = vec3(uv, texture(height_field, uv).r);
  //vec3 base = vec3(0,0,0);

  //vec3 average = vec3(0,0,base.z);
  vec3 average = vec3(0,0,0);
  vec3 up = vec3(0,0,1);
  // left
  if(uv.x - pixelsize.x > 0){
    vec2 new_uv = uv - vec2(pixelsize.x, 0);
    vec3 v = vec3(new_uv, texture(height_field, new_uv).r) - base;
    vec3 r = cross(v, up);
    average = (average + normalize(cross(r,v)))/2;
  }
  // right
  if(uv.x + pixelsize.x < 1){
    vec2 new_uv = uv + vec2(pixelsize.x, 0);
    vec3 v = vec3(new_uv, texture(height_field, new_uv).r) - base;
    vec3 r = cross(v, up);
    average = (average + normalize(cross(r,v)))/2;
    //average += cross(r,v);
  }
  // down
  if(uv.y + pixelsize.x < 1){
    vec2 new_uv = uv + vec2(0, pixelsize.y);
    vec3 v = vec3(new_uv, texture(height_field, new_uv).r);
    vec3 r = cross(v, up);
    //average = (average + normalize(cross(r,v)))/2;
    //average = v;
    average = (average + normalize(cross(r,v)))/2;
  }
  // up
  if(uv.y - pixelsize.x > 0){
    vec2 new_uv = uv - vec2(0, pixelsize.y);
    vec3 v = vec3(new_uv, texture(height_field, new_uv).r) - base;
    vec3 r = cross(v, up);
    //average = (average + normalize(cross(r,v)))/2;
    average = (average + normalize(cross(r,v)))/2;
  }

  average = normalize(average);

  frag_color = vec4((average), 1);
}
