#pragma once

#include <OpenGL/gl3.h>

#include <iterator>
#include <tuple>
#include <vector>

namespace gl{

// Enum for wrapping the OpenGL defines for buffer usage
enum class Usage: GLenum{
  STREAM = GL_STREAM_DRAW,
  STATIC = GL_STATIC_DRAW,
  DYNAMIC= GL_DYNAMIC_DRAW
};

enum class Access: GLenum{
  READ = GL_READ_ONLY,
  WRITE = GL_WRITE_ONLY,
  READ_WRITE = GL_READ_WRITE
};

// Generic buffer class for handeling VBOs, EBOs, TBOs
template<GLuint target, typename T>
class Buffer{
public:

  using Iterator = typename std::vector<T>::iterator;

  Buffer(){}
  Buffer(Usage b, std::vector<T>& data)
    : _usage(b){
    glGenBuffers(1, &_buffer);
    set_data(&data[0], data.size());
  }

  void bind(){
    glBindBuffer(target, _buffer);
  }

  void set_data(T* data, size_t size){
    bind();

    _data = data;
    _size = size;

    glBufferData(target, _size * sizeof(T),
                 _data, GLenum(_usage));
  }

  T* map(Access access){
    bind();
    void* ptr = glMapBuffer(target, GLenum(access));
    return static_cast<T*>(ptr);
  }

  void unmap(){
    bind();
    glUnmapBuffer(target);
  }

  auto size(){return _size;}
  GLuint object(){return _buffer;}

  float& operator[](size_t index){return _data[index];}
  const float& operator[](size_t index) const {return _data[index];}

  void sub_data(Iterator& begin, Iterator& end){
    //auto offset = std::distance(_data_vector.begin(), begin);
    //auto size = std::distance(begin, end);
    //auto data_ptr = &(*begin);
  }

private:
  Usage _usage;
  T* _data;
  size_t _size;
  GLuint _buffer;
};

using VertexBuffer = Buffer<GL_ARRAY_BUFFER, float>;
using ElementBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER, unsigned int>;

};
