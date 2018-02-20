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

TEST_CASE("component assignment", "[entity manager]"){
  weq::EntityManager em;

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
}

TEST_CASE("component masks", "[entity manager]"){
  weq::EntityManager em;

  auto mask = em.generate_component_mask<IntComp, FloatComp>();
  std::cout << mask << std::endl;
}
