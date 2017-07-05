#include <weq/Node.hpp>

Node::Node()
  : _model(1.0f)
{
}

Node::~Node(){
}

void Node::add(Node* child){
  _child = child;
}

void Node::update(float delta_time){
}

void Node::update_all(){
  //update();
  auto ptr = _child;
  while(ptr){
    //ptr->update();
    ptr = _child->_child;
  }
}
