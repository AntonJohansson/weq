#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 v_color[];
out vec4 g_color;

uniform mat4 mvp;
uniform float gridsize;

void main(){
  g_color = v_color[0];

  gl_Position = mvp*(gl_in[0].gl_Position + vec4(-gridsize/2.0,
                                                 -gridsize/2.0,
                                                 0.0,
                                                 0.0));
  EmitVertex();

  gl_Position = mvp*(gl_in[0].gl_Position + vec4(-gridsize/2.0,
                                                 +gridsize/2.0,
                                                 0.0,
                                                 0.0));
  EmitVertex();

  gl_Position = mvp*(gl_in[0].gl_Position + vec4(+gridsize/2.0,
                                                 -gridsize/2.0,
                                                 0.0,
                                                 0.0));
  EmitVertex();

  gl_Position = mvp*(gl_in[0].gl_Position + vec4(+gridsize/2.0,
                                                 +gridsize/2.0,
                                                 0.0,
                                                 0.0));
  EmitVertex();

  EndPrimitive();
}
