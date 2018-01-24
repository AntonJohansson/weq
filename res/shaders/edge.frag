#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D edge_field;
uniform vec2 pixelsize;
uniform float c;
uniform float dt;

void main(){
  float y_value = floor(gl_FragCoord.y);
  float x_value = floor(gl_FragCoord.x);

  if(y_value == 0){
    float u = texture(height_field, vec2(0, v_texcoord.x)).r;
    float g = texture(edge_field, v_texcoord).r;
    float g_new = (c*dt*u + g*(5.0/100.0))/((5/100.0) + c*dt);
    frag_color = vec4(g_new, 0, 0, 0);
    //frag_color = vec4(x_value/100.0, 0, 0, 0);
  }else if(y_value == 1){

    float u = texture(height_field, vec2(1, v_texcoord.x)).r;
    frag_color = vec4(u, 0, 0, 0);
    //frag_color = vec4((100 - x_value)/100.0, 0, 0, 0);
  }else if(y_value == 2){

    float u = texture(height_field, vec2(v_texcoord.x, 0)).r;
    frag_color = vec4(u, 0, 0, 0);
    //frag_color = vec4(x_value/100.0, 0, 0, 0);
  }else if(y_value == 3){

    float u = texture(height_field, vec2(v_texcoord.x, 1)).r;
    frag_color = vec4(u, 0, 0, 0);
    //frag_color = vec4((100 - x_value)/100.0, 0, 0, 0);
  }
}
