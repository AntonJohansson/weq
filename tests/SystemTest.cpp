#include <weq/ecs/SystemManager.hpp>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <iomanip>

using weq::System;

class System1 : public System<System1>{
public:
  void update(weq::Managers& m, f32 dt){
    std::cout << "system1\n";
  }
};

class System2 : public System<System2>{
public:
  void update(weq::Managers& m, f32 dt){
    std::cout << "system2\n";
  }
};

class System3 : public System<System3>{
public:
  void update(weq::Managers& m, f32 dt){
    std::cout << "system3\n";
  }
};

TEST_CASE("create, update", "[System Manager]"){
  weq::SystemManager sm;
  weq::Managers m;
  sm.add<System1>();
  sm.add<System2>();
  sm.add<System3>();

  REQUIRE(sm.size() == 3);

  std::cout << "Expected:\n"
            << std::setw(16) << "system1\n"
            << std::setw(16) << "system2\n"
            << std::setw(16) << "system3\n"
            << "\n";
  sm.update_all(m, 0.0f);

  sm.reset();
  REQUIRE(sm.size() == 0);

  sm.add<System1, 2>();
  sm.add<System2, 1>();
  sm.add<System3, 0>();

  REQUIRE(sm.size() == 3);
  std::cout << "Expected:\n"
            << std::setw(16) << "system3\n"
            << std::setw(16) << "system2\n"
            << std::setw(16) << "system1\n"
            << "\n";
  sm.update_all(m, 0.0f);
}
