#pragma once

#include <glad/glad.h>

#include <iterator>
#include <tuple>
#include <vector>

namespace gl{

// Enum for wrapping the OpenGL defines for buffer usage
enum class Usage : GLenum {
  STREAM_DRAW = GL_STREAM_DRAW,
  STREAM_READ = GL_STREAM_READ,
  STREAM_COPY = GL_STREAM_COPY,
  STATIC_DRAW = GL_STATIC_DRAW,
  STATIC_READ = GL_STATIC_READ,
  STATIC_COPY = GL_STATIC_COPY,
  DYNAMIC_DRAW= GL_DYNAMIC_DRAW,
  DYNAMIC_READ= GL_DYNAMIC_READ,
  DYNAMIC_COPY= GL_DYNAMIC_COPY,
};

enum class Access : GLenum {
  READ = GL_READ_ONLY,
  WRITE = GL_WRITE_ONLY,
  READ_WRITE = GL_READ_WRITE
};

// Generic buffer class for handeling VBOs, EBOs, TBOs
template<GLuint buffer_target, typename T>
class Buffer{
public:

  using Iterator = typename std::vector<T>::iterator;

  Buffer(){}
  Buffer(Usage b, std::vector<T>& data)
    : _usage(b){
    glGenBuffers(1, &_buffer);
    set_data(&data[0], data.size());
  }

  void bind(GLuint target = buffer_target){
    glBindBuffer(target, _buffer);
  }

  void bind(GLuint target, unsigned int index){
    glBindBufferBase(target, index, _buffer);
  }

  void set_data(T* data, size_t size){
    bind();

    _data = data;
    _size = size;

    glBufferData(buffer_target, _size * sizeof(T),
                 _data, GLenum(_usage));
  }

  T* map(Access access){
    bind();
    void* ptr = glMapBuffer(buffer_target, GLenum(access));
    return static_cast<T*>(ptr);
  }

  void unmap(){
    bind();
    glUnmapBuffer(buffer_target);
  }

  auto size(){return _size;}
  GLuint object(){return _buffer;}

  float& operator[](size_t index){return _data[index];}
  const float& operator[](size_t index) const {return _data[index];}

  //void sub_data(Iterator& begin, Iterator& end){
    //auto offset = std::distance(_data_vector.begin(), begin);
    //auto size = std::distance(begin, end);
    //auto data_ptr = &(*begin);
  //}

private:
  Usage _usage;
  T* _data;
  size_t _size;
  GLuint _buffer;
};

using VertexBuffer   = Buffer<GL_ARRAY_BUFFER, float>;
using ElementBuffer  = Buffer<GL_ELEMENT_ARRAY_BUFFER, unsigned int>;
template<typename T>
using FeedbackBuffer = Buffer<GL_TRANSFORM_FEEDBACK_BUFFER, T>;

};
