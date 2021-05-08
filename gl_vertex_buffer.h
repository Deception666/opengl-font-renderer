#ifndef _OPENGL_GL_VERTEX_BUFFER_H_
#define _OPENGL_GL_VERTEX_BUFFER_H_

#include "calling_convention.h"
#include "gl_includes.h"

#include <cstddef>

#ifndef GL_ARRAY_BUFFER_ARB
#define GL_ARRAY_BUFFER_ARB   0x8892
#endif

#ifndef GL_STREAM_DRAW_ARB
#define GL_STREAM_DRAW_ARB    0x88E0
#endif

#ifndef GL_STREAM_READ_ARB
#define GL_STREAM_READ_ARB    0x88E1
#endif

#ifndef GL_STREAM_COPY_ARB
#define GL_STREAM_COPY_ARB    0x88E2
#endif

#ifndef GL_STATIC_DRAW_ARB
#define GL_STATIC_DRAW_ARB    0x88E4
#endif

#ifndef GL_STATIC_READ_ARB
#define GL_STATIC_READ_ARB    0x88E5
#endif

#ifndef GL_STATIC_COPY_ARB
#define GL_STATIC_COPY_ARB    0x88E6
#endif

#ifndef GL_DYNAMIC_DRAW_ARB
#define GL_DYNAMIC_DRAW_ARB   0x88E8
#endif

#ifndef GL_DYNAMIC_READ_ARB
#define GL_DYNAMIC_READ_ARB   0x88E9
#endif

#ifndef GL_DYNAMIC_COPY_ARB
#define GL_DYNAMIC_COPY_ARB   0x88EA
#endif

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
   
   void (CALL_CONV * const glGenBuffers) ( GLsizei, GLuint * );
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
