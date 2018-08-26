#pragma once

// TODO move?
//#include <queue>
//#include <numeric>

template<typename T>
class RunningAverage{
public:
  RunningAverage(int samples = 0)
    : _total(0),
      _samples(samples) {}
  ~RunningAverage(){}

  void add(T t){
    if(_samples != 0 && _data.size() == _samples){
      _total -= _data.front();
      _data.pop();
    }

    _data.push(t);
    _total += t;
  }

  T average(){
    return _total/static_cast<T>(_data.size());
  }

  void clear(){_total = 0.0; _data.clear();}

  T total(){return _total;}
  unsigned int count(){return _data.size();}


private:
  std::queue<T> _data;
  T _total;
  unsigned int _samples;
};
