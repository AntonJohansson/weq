#pragma once

// TODO move?

template<typename T>
class RunningAverage{
public:
  RunningAverage()
    : _total(0),
      _count(0){}
  ~RunningAverage(){}

  void add(T t){_count++; _total += t;}
  void clear(){_total = 0.0; _count = 0;}
  T total(){return _total;}
  unsigned int count(){return _count;}
  T average(){return _total/static_cast<T>(_count);}

private:
  T _total;
  unsigned int _count;
};
