#pragma once

struct MeshData{
  std::vector<float> vertices;
  std::vector<float> color;
  std::vector<float> normals;
  std::vector<float> texcoords;
  std::vector<unsigned int> elements;

  bool has_vertices() const {return !vertices.empty();}
  bool has_color()    const {return !color.empty();}
  bool has_normals()  const {return !normals.empty();}
  bool has_texcoords()const {return !texcoords.empty();}
  bool has_elements() const {return !elements.empty();}
};
