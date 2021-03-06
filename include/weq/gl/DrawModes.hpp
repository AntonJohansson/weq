#pragma once

//#include <glad/glad.h>


namespace weq::gl{

// Abstracts some common OpenGL draw modes.
enum class DrawMode: GLenum {
  POINTS                    = GL_POINTS,
  LINES_STRIP               = GL_LINE_STRIP,
  LINE_LOOP                 = GL_LINE_LOOP,
  LINES                     = GL_LINES,
  LINES_STRIP_ADJACENCY     = GL_LINE_STRIP_ADJACENCY,
  LINES_ADJACENCY           = GL_LINES_ADJACENCY,
  TRIANGLE_STRIP            = GL_TRIANGLE_STRIP,
  TRIANGLE_FAN              = GL_TRIANGLE_FAN,
  TRIANGLES                 = GL_TRIANGLES,
  TRIANGLES_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
  TRIANGLES_ADJACENCY       = GL_TRIANGLES_ADJACENCY,
};

} // namespace weq::gl
