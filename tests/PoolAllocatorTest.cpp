#include <weq/memory/PoolAllocator.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <random>
#include <vector>

#include <iostream>


TEST_CASE("Pool allocator", "[Pool Allocator]"){
  u64 size = 100;

  weq::memory::PoolAllocator<i32> pool(size);
  std::vector<i32*> pool_values;
  std::vector<i32> expected_values;


  // Random
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-5000, 5000);

  SECTION("allocation"){
    for(u64 i = 0; i < size; i++){
      i32 random_int = dis(gen);

      pool_values.push_back(pool.alloc(random_int));

      REQUIRE(pool.size() == i + 1);
      REQUIRE(pool.free_size() == size - (i + 1));

      expected_values.push_back(random_int);
    }

    // check values
    for(u64 i = 0; i < size; i++){
      REQUIRE(*pool_values[i] == expected_values[i]);
    }

    // dealloc
    u64 last_size = pool.size();
    u64 last_free = pool.free_size();
    pool.dealloc(pool_values[0]);
    u64 new_size = pool.size();
    u64 new_free = pool.free_size();

    REQUIRE(new_size == last_size - 1);
    REQUIRE(new_free == last_free + 1);

    // dealloc alloc
    for(int i = 1; i < size; i++){
      pool.dealloc(pool_values[i]);
    }

    REQUIRE(pool.size() == 0);
    REQUIRE(pool.free_size() == size);
  }
}

TEST_CASE("larger data structure", "[Pool Allocator]"){
  struct Data{
    Data(int i, float f, double d)
      : i(i), f(f), d(d){}
    int i;
    float f;
    double d;
  };

  u64 size = 100;
  weq::memory::PoolAllocator<Data> pool(size);

  std::vector<Data*> data_vec;
  for(int i = 0; i < size; i++){
    data_vec.push_back(pool.alloc(1, 5.9f, 10.1));
  }

  for(int i = 0; i < size; i++){
    REQUIRE(data_vec[i]->i == 1);
    REQUIRE(data_vec[i]->f == 5.9f);
    REQUIRE(data_vec[i]->d == 10.1);
  }
}
