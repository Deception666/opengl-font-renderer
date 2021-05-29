#ifndef _OPENGL_GL_VERTEX_BUFFER_H_
#define _OPENGL_GL_VERTEX_BUFFER_H_

#include "calling_convention.h"
#include "gl_includes.h"

#include <cstddef>

namespace opengl {
namespace gl {

class VertexBuffer final
{
public:
   VertexBuffer(
      const GLenum usage );
   ~VertexBuffer( ) noexcept;

   VertexBuffer(
      VertexBuffer && ) noexcept = delete;
   VertexBuffer(
      const VertexBuffer & ) noexcept = delete;

   VertexBuffer & operator = (
      VertexBuffer && ) noexcept = delete;
   VertexBuffer & operator = (
      const VertexBuffer & ) noexcept = delete;

   GLuint GetID( ) const noexcept;
   GLenum GetTarget( ) const noexcept;

   bool SetData(
      const void * const data,
      const size_t data_size ) noexcept;
   template < typename T >
   bool SetData(
      const T * const data,
      const size_t data_size ) noexcept;

   size_t GetSize( ) const noexcept;
   template < typename T >
   size_t GetSize( ) const noexcept;

private:
   GLuint vertex_buffer_;

   size_t size_;
   size_t max_size_;
   
   const GLenum usage_;
   
   void (CALL_CONV * const glCreateBuffers) ( GLsizei, GLuint * );
   void (CALL_CONV * const glDeleteBuffers) ( GLsizei, const GLuint * );
   void (CALL_CONV * const glBindBuffer) ( GLenum, GLuint );
   void (CALL_CONV * const glNamedBufferData) ( GLuint, std::ptrdiff_t, const void *, GLenum );
   void (CALL_CONV * const glNamedBufferSubData) ( GLuint, std::ptrdiff_t, std::ptrdiff_t, const void * );

};

template < typename T >
inline bool VertexBuffer::SetData(
   const T * const data,
   const size_t data_size ) noexcept
{
   return
      SetData(
         static_cast< const void * >(data),
         data_size * sizeof(T));
}

template < typename T >
inline size_t VertexBuffer::GetSize( ) const noexcept
{
   return
      size_ / sizeof(T);
}

}} // namespace opengl::gl

#endif // _OPENGL_GL_VERTEX_BUFFER_H_
