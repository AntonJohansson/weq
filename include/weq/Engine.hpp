#pragma once

#include <entityx/entityx.h>

class ResourceManager;


namespace weq::engine{
namespace ex = entityx;

void initialize();
void main_loop();
ex::Entity create_entity();
ResourceManager* resource_mgr();


float width();
float height();

};
