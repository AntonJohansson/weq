#include <weq/ecs/EntityManager.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <iomanip>

TEST_CASE("Create entity", "[entity manager]"){
  weq::EntityManager em;

  auto e0 = em.create();
  auto e1 = em.create();
  auto e2 = em.create();

  REQUIRE(e0 != e1);
  REQUIRE(e1 != e2);
  REQUIRE(e0 != e2);
}

struct IntComp{
  IntComp(int i) : i(i){}
  int i;
};

struct FloatComp{
  FloatComp(float f) : f(f){}
  float f;
};

TEST_CASE("component assignment and removal", "[entity manager]"){
  weq::EntityManager em;

  // Component creation
  auto e0 = em.create();
  auto int_c = em.assign<IntComp>(e0, 1);

  REQUIRE( int_c->i == 1);
  REQUIRE( em.has_component<IntComp>(e0));
  REQUIRE(!em.has_component<FloatComp>(e0));

  auto float_c = em.assign<FloatComp>(e0, 2.3f);

  REQUIRE(int_c->i   == 1);
  REQUIRE(float_c->f == 2.3f);
  REQUIRE(em.has_component<IntComp>(e0));
  REQUIRE(em.has_component<FloatComp>(e0));

  // Component removal
  em.remove<IntComp>(e0);
  REQUIRE(!em.has_component<IntComp>(e0));
  REQUIRE( em.has_component<FloatComp>(e0));
}

TEST_CASE("component masks generation", "[entity manager]"){
  weq::EntityManager em;

  auto mask = em.generate_component_mask<IntComp, FloatComp>();
  REQUIRE(mask.count() == 2);
}

TEST_CASE("entity_with_components, get", "[entity manager]"){
  weq::EntityManager em;

  u64 n = 10;
  // Create n components with IntComp and FloatComp
  for(u64 i = 0; i < n; i++){
    auto e = em.create();
    em.assign<IntComp>(e, 1);
    em.assign<FloatComp>(e, 2.3f);
  }
  // Create n components with IntComp
  for(u64 i = 0; i < n; i++){
    auto e = em.create();
    em.assign<IntComp>(e, 1);
  }
  // Create n components with FloatComp
  for(u64 i = 0; i < n; i++){
    auto e = em.create();
    em.assign<FloatComp>(e, 2.3f);
  }

  REQUIRE(em.size() == 3*n);

  // Check int comp
  auto vec = em.entities_with_components<IntComp>();
  REQUIRE(vec.size() == 2*n);
  for(auto id : vec){
    REQUIRE(em.has_component<IntComp>(id));
    REQUIRE(em.get<IntComp>()->i == 1);
  }

  // Check float comp
  vec = em.entities_with_components<FloatComp>();
  REQUIRE(vec.size() == 2*n);
  for(auto id : vec){
    REQUIRE(em.has_component<FloatComp>(id));
    REQUIRE(em.get<FloatComp>()->f == 2.3f);
  }
}
