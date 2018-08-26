#include <weq/gl/VertexFormat.hpp>

//#include <algorithm>

namespace weq::gl{

namespace{

// Returns size of OpenGL types.
  static size_t get_size(Type type){
    switch(type){
    case Type::FLOAT: return sizeof(GLfloat); break;
    case Type::DOUBLE: return sizeof(GLdouble); break;
    case Type::INT: return sizeof(GLint); break;
    case Type::UINT: return sizeof(GLuint); break;
    default: return 0;
    }
}

}

VertexFormat::VertexFormat(std::vector<VertexAttribute> a)
  : attributes(a)
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

bool VertexFormat::has(const std::string& name){
  auto it = std::find_if(attributes.begin(),
                         attributes.end(),
                         [&name](const VertexAttribute& attribute){
                           return attribute.attribute == name;
                         });

  return it != attributes.end();
}


} // namespace weq::gl
