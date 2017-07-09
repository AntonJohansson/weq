#pragma once

#include <string>

class Node;

namespace engine{

void initialize();
void main_loop();
void add_node(Node* node);

float width();
float height();

};
