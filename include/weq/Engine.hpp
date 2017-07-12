#pragma once

#include <entityx/entityx.h>

class Node;


namespace weq::engine{
namespace ex = entityx;

void initialize();
void main_loop();
ex::Entity create_entity();

float width();
float height();

};
