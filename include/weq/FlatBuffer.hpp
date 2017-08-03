#pragma once

#include <functional>
#include <algorithm>
#include <iostream>
#include <cassert>

// TODO remove?

template<typename T>
class FlatBuffer{
public:
  using Iterator = T*;
  using ConstIterator = const T*;

  FlatBuffer(unsigned int w, unsigned int h)
    : _width(w),
      _height(h),
      _size(w*h){
    _data = new T[_size];
  }

  FlatBuffer(const FlatBuffer<T>& buffer)
    : _width(buffer.width()),
      _height(buffer.height()),
      _size(buffer.size()){
    _data = new T[_size];
    // copy buffer, +1 required to copy entire buffer for some reason
    std::copy(buffer.begin(), buffer.end() + 1, _data);
  }

  FlatBuffer<T>& operator=(const FlatBuffer<T>& buffer){
    if(this != &buffer){
      if(size() != buffer.size()){
        delete _data;
        _data = new T[buffer.size()];
        _size = buffer.size();
      }

      std::copy(buffer.begin(), buffer.end() + 1, _data);
      _width = buffer.width();
      _height = buffer.height();
    }

    return *this;
  }

  ~FlatBuffer(){
    delete[] _data;
  }

  void fill(T value){
    for(unsigned int x = 0; x < _width; x++){
      for(unsigned int y = 0; y < _height; y++){
        _data[to_index(x, y)] = value;
      }
    }
  }

  size_t to_index(size_t x, size_t y){
    assert(x*_width + y < _size);
    return x*_width + y;
  }

  Iterator to_iterator(size_t x, size_t y){
    return &_data[to_index(x, y)];
  }

  T* data(){return _data;}
  auto size() const {return _size;}
  auto width() const {return _width;}
  auto height() const {return _height;}

  void sub_data(size_t x0, size_t y0, size_t x1, size_t y1, std::function<void(T&)> func){
    for(auto i = to_index(x0, y0); i < to_index(x1, y1); i++){
      func(_data[i]);
    }
  }

  T& operator()(size_t x, size_t y){
    return at(x, y);
  }

  T& at(size_t x, size_t y){
    return _data[to_index(x,y)];
  }

  FlatBuffer<T>& operator+=(const FlatBuffer<T>& rhs){
    for(int x = 0; x < _width; x++){
      for(int y = 0; y < _height; y++){
        *this(x, y) += rhs(x, y);
      }
    }

    return *this;
  }

  Iterator begin(){return &_data[0];}
  Iterator end(){return &_data[_size - 1];}
  ConstIterator begin() const {return &_data[0];}
  ConstIterator end() const {return &_data[_size - 1];}

private:
  unsigned int _width;
  unsigned int _height;
  unsigned int _size;
  T* _data;
};

template<typename T>
inline FlatBuffer<T> operator+(FlatBuffer<T> lhs, const FlatBuffer<T>& rhs){
  lhs += rhs;
  return lhs;
}
