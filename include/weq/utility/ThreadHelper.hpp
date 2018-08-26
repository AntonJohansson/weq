#pragma once

//#include <functional>
//#include <thread>

namespace weq{

void on_thread_exit(std::function<void()> f){
  thread_local struct Exiter{
    ~Exiter(){
      func();
    }

    void set(std::function<void()> f){
      func = f;
    }

    std::function<void()> func;
  } exiter;

  exiter.set(f);
}

}
