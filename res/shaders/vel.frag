#version 410

in vec2 v_texcoord;

out vec4 frag_color;

uniform sampler2D height_field;
uniform sampler2D vel_field; // TEMPORARY - addition can be performed by OpenGL blending.
uniform sampler2D ri_field;
uniform sampler2D edge_field;
uniform vec2 pixelsize;
uniform vec2 gridsize;
uniform float dt;
uniform float c;
uniform bool radiate_edge;

void main(){
  if(radiate_edge){
    float wave_v = c/texture(ri_field, v_texcoord).r;
    float r = wave_v*wave_v/(gridsize.x*gridsize.x);

    float w, e, n, s, t;

    if(v_texcoord.x - pixelsize.x < 0){
      w = texture(edge_field, vec2(v_texcoord.y , 0)).r;
    }else{
      w = texture(height_field, vec2(v_texcoord.x - pixelsize.x,
                                     v_texcoord.y)).r;
    }

    if(v_texcoord.x + pixelsize.x > 1.0){
      //e = texture(edge_field, vec2(v_texcoord.y , 1.0*(1.0/4.0 + 0.5/4.0))).r;
      e = texture(height_field, vec2(v_texcoord.x + pixelsize.x,
                                     v_texcoord.y)).r;
    }else{
      e = texture(height_field, vec2(v_texcoord.x + pixelsize.x,
                                     v_texcoord.y)).r;
    }

    if(v_texcoord.y - pixelsize.y < 0){
      //n = texture(edge_field, vec2(v_texcoord.x, 2.0*(1.0/4.0) + 0.5/4.0)).r;
      n = texture(height_field, vec2(v_texcoord.x,
                                     v_texcoord.y - pixelsize.y)).r;
    }else{
      n = texture(height_field, vec2(v_texcoord.x,
                                     v_texcoord.y - pixelsize.y)).r;
    }

    if(v_texcoord.y + pixelsize.y > 1.0){
      //s = texture(edge_field, vec2(v_texcoord.x, 3.0*(1.0/4.0) + 0.5/4.0)).r;
      s = texture(height_field, vec2(v_texcoord.x,
                                     v_texcoord.y + pixelsize.y)).r;
    }else{
      s = texture(height_field, vec2(v_texcoord.x,
                                     v_texcoord.y + pixelsize.y)).r;
    }


    t = texture(height_field, v_texcoord).r;

    float k = 1.0;
    float f = k*r*(w + e + n + s - 4.0*t);

    float dv = f*dt;
    float v = texture(vel_field, v_texcoord).r + dv;

    frag_color = vec4(v, 0, 0 ,0); // Blend mode will add to velocity texture.

  }else{
    float wave_v = c/texture(ri_field, v_texcoord).r;
    float r = wave_v*wave_v/(gridsize.x*gridsize.x);

    float w, e, n, s, t;

    w = texture(height_field, vec2(v_texcoord.x - pixelsize.x,
                                     v_texcoord.y)).r;

    e = texture(height_field, vec2(v_texcoord.x + pixelsize.x,
                                   v_texcoord.y)).r;

    n = texture(height_field, vec2(v_texcoord.x,
                                   v_texcoord.y + pixelsize.y)).r;

    s = texture(height_field, vec2(v_texcoord.x,
                                   v_texcoord.y - pixelsize.y)).r;

    t = texture(height_field, v_texcoord).r;

    float k = 1.0;
    float f = k*r*(w + e + n + s - 4.0*t);

    float dv = f*dt;
    float v = texture(vel_field, v_texcoord).r + dv;

    frag_color = vec4(v, 0, 0 ,0); // Blend mode will add to velocity texture.
  }
  // Even if we read and write to the same texture, changes will not be
  // written after the entire draw call is complete -> no need for
  // double buffering (yay).
}
