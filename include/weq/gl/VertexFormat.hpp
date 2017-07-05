#pragma once

#include <OpenGL/gl3.h>

#include <string>
#include <vector>

namespace gl{

enum Type{
  FLOAT = GL_FLOAT,
  DOUBLE = GL_DOUBLE,
  INT = GL_INT,
  UINT = GL_UNSIGNED_INT
};

static size_t get_size(Type type){
  switch(type){
  case FLOAT: return sizeof(float); break;
  case DOUBLE: return sizeof(double); break;
  case INT: return sizeof(int); break;
  case UINT: return sizeof(unsigned int); break;
  default: return 0;
  }
}

struct VertexComponent{
  VertexComponent(std::string attribute, Type type, unsigned int size)
    : attribute(attribute)
    , type(type)
    , size(size)
    , offset(0)
  {}

  std::string attribute;
  Type type;
  unsigned int size;
  unsigned int offset;
};

struct VertexFormat{
  VertexFormat(std::vector<VertexComponent> c)
    : components(c)
    , stride(0)
    , vertex_size(0)
  {
    unsigned int offset = 0;

    for(auto& v : components){
      auto size = get_size(v.type);
      vertex_size += v.size;

      if(components.size() > 1){
        stride += v.size * size;
        v.offset = offset;

        offset += v.size * size;
      }
    }
  }

  size_t size(){return components.size();}
  VertexComponent& operator[](size_t index){return components[index];}
  const VertexComponent& operator[](size_t index) const {return components[index];}

  std::vector<VertexComponent> components;
  unsigned int stride;
  unsigned int vertex_size;
};

namespace formats{

const static VertexFormat VNCT = {{
  {"position", FLOAT, 3},
  {"normal", FLOAT, 3},
  {"color", FLOAT, 4},
  {"texcoord", FLOAT, 3}
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
    {"nomal", FLOAT, 3}
}};

const static VertexFormat C = {{
    {"color", FLOAT, 4}
}};

const static VertexFormat T = {{
    {"texcoord", FLOAT, 3}
}};

}

};
