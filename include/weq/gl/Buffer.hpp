#pragma once

//#include <glad/glad.h>
//#include <spdlog/spdlog.h>

//#include <vector>

namespace weq::gl{

// Enum for wrapping the OpenGL enums pertaining to
enum class Usage: GLenum {
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

// Enum for wrapping accessing modes to mapped memory regions.
enum class Access: GLenum {
  READ = GL_READ_ONLY,
  WRITE = GL_WRITE_ONLY,
  READ_WRITE = GL_READ_WRITE
};

// Generic buffer class for handling VBOs and EBOs. The
// buffer_target template param. specifies the OpenGL
// buffer target (GL_ARRAY_BUFFER, ...)
// The typename specifies the buffer type.
template<GLenum buffer_target, typename T>
class Buffer{
public:
  // Does absolutely nothing.
  Buffer(Usage b = Usage::STATIC_DRAW)
    : _usage(b),
      _data(nullptr),
      _size(0),
      _buffer(0){
    glGenBuffers(1, &_buffer);
    // Create an empty texture?
    set_data(nullptr, 0);
  }

  // Generates the actual OpenGL buffer and calls
  // set_data(...) which uses the data vector and
  // buffer usage.
  Buffer(Usage b, std::vector<T>& data)
    : _usage(b),
      _data(nullptr),
      _size(0),
      _buffer(0){
    glGenBuffers(1, &_buffer);
    set_data(&data[0], data.size());
  }

  // Deletes the resoruces for the OpenGL buffer.
  ~Buffer(){
    if(glIsBuffer(_buffer) == GL_TRUE){
      glDeleteBuffers(1, &_buffer);
    }
  }

  // Copying not allowed.
  Buffer(const Buffer<buffer_target, T>&) = delete;

  // Swap constructor
  Buffer(Buffer<buffer_target, T>&& rhs)
    : _usage(rhs._usage),
      _data(rhs._data),
      _size(rhs._size),
      _buffer(rhs._buffer){
  }


  Buffer<buffer_target, T>& operator=(Buffer<buffer_target, T>&) = delete;

  Buffer<buffer_target, T>& operator=(Buffer<buffer_target, T>&& rhs){
    using std::swap;
    swap(_usage, rhs._usage);
    swap(_data, rhs._data);
    swap(_size, rhs._size);
    swap(_buffer, rhs._buffer);
    return *this;
  }

  // Binds the buffer for use with OpenGL functions,
  // this method asumes the specified template target
  // by default.
  void bind(GLenum target = buffer_target) const {
    glBindBuffer(target, _buffer);
  }

  // This function binds the buffer at a specific binding
  // point as referenced by the index.
  void bind(GLenum target, unsigned int index) const {
    glBindBufferBase(target, index, _buffer);
  }

  // Sets the buffer data calling glBufferData, this function
  // does not invalidate the buffer for more efficient memory
  // replacement.
  // Automatically calls bind().
  void set_data(T* data, size_t size){
    bind();

    _data = data;
    _size = size;

    glBufferData(buffer_target, _size * sizeof(T),
                 _data, GLenum(_usage));
  }

  // Overload of set_data(T*,size_t) making life a bit easier.
  void set_data(std::vector<T>& data){
    set_data(&data[0], data.size());
  }

  // Maps the GPU buffer to a memory region on the CPU so that
  // it can be easily modified.
  // Parameter specifies write access.
  // Automatically calls bind().
  // TODO does currently not support mapped ranges.
  T* map(Access access){
    bind();

    void* ptr = glMapBuffer(buffer_target, GLenum(access));

    if(ptr == nullptr){
      //spdlog::get("console")->error("Failed to map memory region!");
      //TODO should I throw?
    }

    return static_cast<T*>(ptr);
  }

  // Binds the buffer and unmaps the CPU side memory region.
  // Invalidates pointer to the memory region.
  void unmap(){
    bind();

    if(glUnmapBuffer(buffer_target) == GL_FALSE){
      //spdlog::get("console")->error("Failed to unmap memory region!");
      // TODO throw?
    }
  }

  // Returns the current size of the buffer.
  size_t size() const {return _size;}

  // Returns the OpenGL int handle.
  GLuint object() const {return _buffer;}

  // Overloaded operators to access the underlying data,
  // this assumes that the buffer has not been changed
  // after the data was set.
  // TODO remove?
  float& operator[](size_t index){return _data[index];}
  const float& operator[](size_t index) const {return _data[index];}

private:
  Usage _usage;
  T* _data;
  size_t _size;
  GLuint _buffer;
};

// Defines some common template specifications of Buffer.
using VertexBuffer   = Buffer<GL_ARRAY_BUFFER, float>;
using ElementBuffer  = Buffer<GL_ELEMENT_ARRAY_BUFFER, unsigned int>;


// TODO is this set up correctly?
template<typename T>
using FeedbackBuffer = Buffer<GL_TRANSFORM_FEEDBACK_BUFFER, T>;

} // namespace weq::gl
