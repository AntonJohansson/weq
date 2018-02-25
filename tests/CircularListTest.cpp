#include <weq/memory/CircularList.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <iomanip>

TEST_CASE("circular tests", "[Circular List]"){
  weq::memory::CircularList<int> list;

  SECTION("add/remove/for_each"){
    // Create
    auto i1 = list.add(1);
    auto i2 = list.add(2);
    auto i3 = list.add(3);

    REQUIRE(list.size() == 3);
    REQUIRE(list.head()->data == 1);

    // for_each
    list.for_each([](auto node){
        std::cout << std::setw(16) << node->data << std::endl;
      });

    // Remove
    list.remove(i1);
    list.remove(i2);
    list.remove(i3);

    REQUIRE(list.size() == 0);
  }
}
