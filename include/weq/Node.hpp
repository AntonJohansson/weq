#pragma once

#include <weq/gl/VertexArray.hpp>
#include <weq/gl/Shader.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Node{
public:
  Node();
  ~Node();

  void add(Node* child);

  virtual void update(float delta_time);
  void update_all();

//protected:
  // friend class renderer
  gl::Shader* _shader; // Should only be an id, missing resource system...
  std::vector<gl::VertexArray> _vaos; // Should also be nothing but an id
  gl::ElementBuffer _ebo;
  glm::mat4 _model;
  Node* _child;
  Node* _base;
};
