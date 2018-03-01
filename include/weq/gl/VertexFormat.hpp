#pragma once

#include <glad/glad.h>

#include <string>
#include <vector>

namespace weq::gl{

// Some standard abstractions of OpenGL types
enum class Type: GLenum{
  FLOAT = GL_FLOAT,
  DOUBLE = GL_DOUBLE,
  INT = GL_INT,
  UINT = GL_UNSIGNED_INT
};

// Struct associating an attribute name with a type and the number
// of values of that type.
// The offset member variable is set by the vertex format struct,
// since knowledge of other attributes in a format is
// necessary.
struct VertexAttribute{
  VertexAttribute(std::string attribute, Type type, unsigned int length)
    : attribute(attribute)
    , type(type)
    , length(length)
    , offset(0)
  {}

  std::string attribute;
  Type type;
  unsigned int length;
  unsigned int offset;
};

// Struct that combines VertexComponents to form a describtion of
// a single vertex in a format.
struct VertexFormat{
  // Empty constructor.
  VertexFormat(){}

  // Calculates attribute offset, format length (in values), and
  // the vertex stride (size in bytes).
  VertexFormat(std::vector<VertexAttribute> a);

  // Returns true if the format contains a provided attribute.
  bool has(const std::string& name);

  // Returns the number of attributes in the format.
  size_t attribute_count(){return attributes.size();}

  // Overload to access attributes in an array-like fashion.
  VertexAttribute& operator[](size_t index){return attributes[index];}
  const VertexAttribute& operator[](size_t index) const {return attributes[index];}

  std::vector<VertexAttribute> attributes;
  unsigned int stride;
  unsigned int format_length;
};


// Defines some commonly used formats.
namespace format{

const static VertexFormat VNCT = {{
    {"position", Type::FLOAT, 3},
    {"normal",   Type::FLOAT, 3},
    {"color",    Type::FLOAT, 4},
    {"texcoord", Type::FLOAT, 2}
  }};

const static VertexFormat VNC = {{
    {"position", Type::FLOAT, 3},
    {"normal",   Type::FLOAT, 3},
    {"color",    Type::FLOAT, 4},
  }};

const static VertexFormat VNT = {{
    {"position", Type::FLOAT, 3},
    {"normal",   Type::FLOAT, 3},
    {"texcoord", Type::FLOAT, 2}
  }};

const static VertexFormat VC = {{
    {"position", Type::FLOAT, 3},
    {"color",    Type::FLOAT, 4}
  }};

const static VertexFormat VT = {{
    {"position", Type::FLOAT, 3},
    {"texcoord", Type::FLOAT, 2}
  }};

const static VertexFormat NT = {{
    {"normal",   Type::FLOAT, 3},
    {"texcoord", Type::FLOAT, 2}
  }};

const static VertexFormat VN = {{
    {"position",   Type::FLOAT, 3},
    {"normal",     Type::FLOAT, 3}
  }};


const static VertexFormat NC = {{
    {"normal", Type::FLOAT, 3},
    {"color",  Type::FLOAT, 4}
  }};

const static VertexFormat V = {{
    {"position", Type::FLOAT, 3}
  }};

const static VertexFormat N = {{
    {"normal", Type::FLOAT, 3}
  }};

const static VertexFormat C = {{
    {"color", Type::FLOAT, 4}
  }};

const static VertexFormat T = {{
    {"texcoord", Type::FLOAT, 2}
  }};

} // namespace format

} // namespace weq::gl
