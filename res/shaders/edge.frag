#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform vec2 pixelsize;
uniform float c;
uniform float dt;

void main(){
  float height = texture(height_field, v_texcoord).r;

  if(v_texcoord.x < pixelsize.x || v_texcoord.x > 1.0 - pixelsize.x ||
     v_texcoord.y < pixelsize.y || v_texcoord.y > 1.0 - pixelsize.y){
    vec2 dist_to_center = normalize(vec2(0.5, 0.5) - v_texcoord);

    vec2 final = dist_to_center;
    if(abs(dist_to_center.x) > abs(dist_to_center.y)){
      final = vec2(dist_to_center.x/dist_to_center.x, 0);
    }else if(abs(dist_to_center.x) < abs(dist_to_center.y)){
      final = vec2(0, dist_to_center.y/dist_to_center.y);
    }

    float last_height = texture(height_field, v_texcoord + dot(pixelsize, final)).r;
    height = (c*dt*height + last_height*pixelsize.x)/(pixelsize.x + c*dt);
    //height = (c*last_height + height)/(1.0 + c);
  }

  frag_color = vec4(height, 0, 0, 0);
}
