#pragma once

#include <glad/glad.h>

#include <string>
#include <vector>
#include <algorithm>

namespace gl{

// Some standard abstractions of OpenGL types
// TODO extern?
enum Type{
  FLOAT = GL_FLOAT,
  DOUBLE = GL_DOUBLE,
  INT = GL_INT,
  UINT = GL_UNSIGNED_INT
};

// Returns size of OpenGL types.
static size_t get_size(Type type){
  switch(type){
  case FLOAT: return sizeof(float); break;
  case DOUBLE: return sizeof(double); break;
  case INT: return sizeof(int); break;
  case UINT: return sizeof(unsigned int); break;
  default: return 0;
  }
}

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
  // Calculates attribute offset, format length (in values), and
  // the vertex stride (size in bytes).
  VertexFormat(std::vector<VertexAttribute> attributes)
    : attributes(attributes)
    , stride(0)
    , format_length(0)
  {
    unsigned int offset_count = 0;

    for(auto& attribute : attributes){
      auto size = get_size(attribute.type);
      format_length += attribute.length;

      // If the format only contains a single attribute,
      // don't update stride and offset (not necessary).
      if(attributes.size() > 1){
        stride += attribute.length * size;
        attribute.offset = offset_count;

        offset_count += attribute.length * size;
      }
    }
  }

  // Returns true if the format contains a provided attribute.
  bool has(const std::string& name){
    auto it = std::find_if(attributes.begin(),
                 attributes.end(),
                 [&name](const VertexAttribute& attribute){
        return attribute.attribute == name;
      });

    return it != attributes.end();
  }

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
  {"position", FLOAT, 3},
  {"normal", FLOAT, 3},
  {"color", FLOAT, 4},
  {"texcoord", FLOAT, 2}
}};

const static VertexFormat VNC = {{
    {"position", FLOAT, 3},
    {"normal", FLOAT, 3},
    {"color", FLOAT, 4},
}};

const static VertexFormat VNT = {{
    {"position", FLOAT, 3},
    {"normal", FLOAT, 3},
    {"texcoord", FLOAT, 3}
}};

const static VertexFormat VC = {{
  {"position", FLOAT, 3},
  {"color", FLOAT, 4}
}};

const static VertexFormat VT = {{
  {"position", FLOAT, 3},
  {"texcoord", FLOAT, 3}
}};

const static VertexFormat NT = {{
    {"normal", FLOAT, 3},
    {"texcoord", FLOAT, 3}
}};

const static VertexFormat NC = {{
    {"normal", FLOAT, 3},
    {"color", FLOAT, 4}
}};

const static VertexFormat V = {{
    {"position", FLOAT, 3}
}};

const static VertexFormat N = {{
    {"normal", FLOAT, 3}
}};

const static VertexFormat C = {{
    {"color", FLOAT, 4}
}};

const static VertexFormat T = {{
    {"texcoord", FLOAT, 3}
}};

}

} // namespace gl
