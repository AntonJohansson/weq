#include <weq/ecs/EventManager.hpp>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

struct Event{
  Event(int i, float f)
    : i(i),
      f(f){}

  int i;
  float f;
};

struct Data : public weq::Receiver{
  static int count;
  void receive(const Event& e){
    count++;
    REQUIRE(e.i == 1);
    REQUIRE(e.f == 2.3f);
  }
};
int Data::count = 0;

TEST_CASE("subscribe", "[event]"){
  weq::EventManager em;
  int tests = 100;

  std::vector<Data> data_vec;
  // Reserve since expantion of vector will call destructor on
  // Receiver causing disconnection of receivers
  data_vec.reserve(tests);

  for(int i = 0; i < tests; i++){
    data_vec.emplace_back();
    em.subscribe<Event>(data_vec.back());
  }

  em.emit<Event>(1, 2.3f);

  REQUIRE(Data::count == tests);


  // Unsub first half
  for(int i = 0; i < tests/2; i++){
    em.unsubscribe<Event>(data_vec[i]);
  }

  em.emit<Event>(1, 2.3f);

  REQUIRE(Data::count == tests + (tests - tests/2));
}
