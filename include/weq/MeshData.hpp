#pragma once

//#include <vector>
#include <weq/gl/VertexFormat.hpp>

namespace weq{

// Represents mesh data in an engine-friendly format
// as described by gl::VertexFormat.
struct MeshData{
  MeshData(){
  }

  MeshData(gl::VertexFormat format)
    : format(format){
  }

  gl::VertexFormat format;

  std::vector<float> interleaved;
  std::vector<unsigned int> elements;

  bool has_data() const {return !interleaved.empty();}
  bool has_elements() const {return !elements.empty();}
};

} // namespace weq
