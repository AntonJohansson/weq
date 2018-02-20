#include <weq/memory/PoolAllocator.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <random>
#include <vector>

#include <iostream>

using weq::memory::RESIZE;
using weq::memory::NO_RESIZE;

TEST_CASE("Pool alloc reserve", "[Pool Allocator]"){
  SECTION("RESIZE"){
    weq::memory::PoolAllocator<i32> pool(2, RESIZE);

    REQUIRE(pool.size()       == 0);
    REQUIRE(pool.blocks()     == 1);
    REQUIRE(pool.capacity()   == 2);
    REQUIRE(pool.block_size() == 2);
    REQUIRE(pool.free()       == 2);

    pool.alloc(1);

    REQUIRE(pool.size()       == 1);
    REQUIRE(pool.blocks()     == 1);
    REQUIRE(pool.capacity()   == 2);
    REQUIRE(pool.block_size() == 2);
    REQUIRE(pool.free()       == 1);

    pool.alloc(1);

    REQUIRE(pool.size()       == 2);
    REQUIRE(pool.blocks()     == 1);
    REQUIRE(pool.capacity()   == 2);
    REQUIRE(pool.block_size() == 2);
    REQUIRE(pool.free()       == 0);

    pool.alloc(1);

    REQUIRE(pool.size()       == 3);
    REQUIRE(pool.blocks()     == 2);
    REQUIRE(pool.capacity()   == 4);
    REQUIRE(pool.block_size() == 2);
    REQUIRE(pool.free()       == 1);
  }

  SECTION("NO_RESIZE"){
    weq::memory::PoolAllocator<i32> pool(2, NO_RESIZE);

    REQUIRE(pool.size()       == 0);
    REQUIRE(pool.blocks()     == 1);
    REQUIRE(pool.capacity()   == 2);
    REQUIRE(pool.block_size() == 2);
    REQUIRE(pool.free()       == 2);

    pool.alloc(1);

    REQUIRE(pool.size()       == 1);
    REQUIRE(pool.blocks()     == 1);
    REQUIRE(pool.capacity()   == 2);
    REQUIRE(pool.block_size() == 2);
    REQUIRE(pool.free()       == 1);

    pool.alloc(1);

    REQUIRE(pool.size()       == 2);
    REQUIRE(pool.blocks()     == 1);
    REQUIRE(pool.capacity()   == 2);
    REQUIRE(pool.block_size() == 2);
    REQUIRE(pool.free()       == 0);

    REQUIRE_THROWS(pool.alloc(1));
  }
}

TEST_CASE("Pool allocator (no resize)", "[Pool Allocator]"){
  u64 size = 100;

  weq::memory::PoolAllocator<i32> pool(size, NO_RESIZE);
  std::vector<i32*> pool_values;
  std::vector<i32> expected_values;

  // Random
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-5000, 5000);

  SECTION("alloc/dealloc"){
    for(u64 i = 0; i < size; i++){
      i32 random_int = dis(gen);

      pool_values.push_back(pool.alloc(random_int));

      REQUIRE(pool.size() == i + 1);
      REQUIRE(pool.free() == size - (i + 1));

      expected_values.push_back(random_int);
    }

    // check values
    for(u64 i = 0; i < size; i++){
      REQUIRE(*pool_values[i] == expected_values[i]);
    }

    // Check throws
    REQUIRE_THROWS(pool.alloc(1));

    // dealloc
    u64 last_size = pool.size();
    u64 last_free = pool.free();
    pool.dealloc(pool_values[0]);
    u64 new_size = pool.size();
    u64 new_free = pool.free();

    REQUIRE(new_size == last_size - 1);
    REQUIRE(new_free == last_free + 1);

    // dealloc alloc
    for(u64 i = 1; i < size; i++){
      pool.dealloc(pool_values[i]);
    }

    REQUIRE(pool.size() == 0);
    REQUIRE(pool.free() == size);
  }
}

TEST_CASE("larger data structure (no resize)", "[Pool Allocator]"){
  struct Data{
    Data(int i, float f, double d)
      : i(i), f(f), d(d){}
    int i;
    float f;
    double d;
  };

  u64 size = 100;
  weq::memory::PoolAllocator<Data> pool(size, NO_RESIZE);

  std::vector<Data*> data_vec;
  for(u64 i = 0; i < size; i++){
    data_vec.push_back(pool.alloc(1, 5.9f, 10.1));
  }

  for(u64 i = 0; i < size; i++){
    REQUIRE(data_vec[i]->i == 1);
    REQUIRE(data_vec[i]->f == 5.9f);
    REQUIRE(data_vec[i]->d == 10.1);
  }
}
