#include <weq/ecs/Signal.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("multiple lambdas", "[signal]"){
  u64 size = 100;
  weq::Signal<void(int)> sig;

  int sum = 0;

  for(u64 i = 0; i < size; i++){
    sig.connect([&sum](int i){sum += i;});
  }

  sig.emit(5);
  REQUIRE(sum == size*5);
}

TEST_CASE("member functions", "[signal]"){
  struct Data{
    int sum = 0;
    void receive(int i){
      sum += i;
    }
  };

  u64 size = 100;
  weq::Signal<void(int)> sig;

  Data data;

  for(u64 i = 0; i < size; i++){
    using std::placeholders::_1;
    sig.connect(std::bind(&Data::receive, &data, _1));
  }

  sig.emit(5);
  REQUIRE(data.sum == size*5);
}

TEST_CASE("member functions different signatures", "[signal]"){
  struct Data{
    int i_sum = 0;
    float f_sum = 0.0f;

    void receive(int i){
      i_sum += i;
    }

    void receive(float f){
      f_sum += f;
    }
  };

  u64 size = 100;
  weq::Signal<void(int)> i_sig;
  weq::Signal<void(float)> f_sig;

  Data data;

  for(u64 i = 0; i < size; i++){
    using std::placeholders::_1;
    void (Data::*iptr)(int) = &Data::receive;
    void (Data::*fptr)(float) = &Data::receive;
    i_sig.connect(std::bind(iptr, &data, _1));
    f_sig.connect(std::bind(fptr, &data, _1));
  }

  i_sig.emit(5);
  f_sig.emit(5.5f);
  REQUIRE(data.i_sum == size*5);
  REQUIRE(data.f_sum == size*5.5f);
}

TEST_CASE("connect/disconnect"){
  u64 size = 100;
  weq::Signal<void(int)> sig;

  std::vector<weq::Signal<void(int)>::Node*> connections;

  int sum = 0;
  for(u64 i = 0; i < size; i++){
    auto con = sig.connect([&sum](int i){sum += i;});
    connections.push_back(con);
  }

  sig.emit(1);

  REQUIRE(sum == size*1);

  // Unsub half of the connections
  for(u64 i = 0; i < size/2; i++){
    sig.disconnect(connections[i]);
  }

  sum = 0;
  sig.emit(1);

  REQUIRE(sum == size - size/2);

  // Unsub all but one
  for(u64 i = size/2; i < size - 1; i++){
    sig.disconnect(connections[i]);
  }

  sum = 0;
  sig.emit(1);

  REQUIRE(sum == 1);

  // Unsub the last one
  sig.disconnect(connections.back());

  sum = 0;
  sig.emit(1);

  REQUIRE(sum == 0);
}

TEST_CASE("Priority order", "[signal]"){
  u64 size = 100;
  weq::Signal<void()> sig;

  std::string result = "";
  auto add_a = [&result](){result += "a";};
  auto add_b = [&result](){result += "b";};
  auto add_c = [&result](){result += "c";};

  sig.connect(add_a, 0);
  sig.emit();

  REQUIRE(result == "a");
  result = "";
  sig.reset();

  sig.connect(add_a, 0);
  sig.connect(add_b, 1);
  sig.emit();

  REQUIRE(result == "ab");
  result = "";
  sig.reset();

  sig.connect(add_b, 1);
  sig.connect(add_a, 0);
  sig.emit();

  REQUIRE(result == "ab");
  result = "";
  sig.reset();

  sig.connect(add_a, 0);
  sig.connect(add_b, 0);
  sig.connect(add_c, 0);
  sig.emit();

  REQUIRE(result == "abc");
  result = "";
  sig.reset();

  sig.connect(add_a, 0);
  sig.connect(add_b, 1);
  sig.connect(add_c, 2);
  sig.emit();

  REQUIRE(result == "abc");
  result = "";
  sig.reset();

  sig.connect(add_c, 2);
  sig.connect(add_b, 1);
  sig.connect(add_a, 0);
  sig.emit();

  REQUIRE(result == "abc");
  result = "";
  sig.reset();

  sig.connect(add_a, 0);
  sig.connect(add_c, 2);
  sig.connect(add_b, 1);
  sig.emit();

  REQUIRE(result == "abc");
  result = "";
  sig.reset();
}
