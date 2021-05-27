#ifndef _OPENGL_GL_UNIFORM_BUFFER_H_
#define _OPENGL_GL_UNIFORM_BUFFER_H_

#include "calling_convention.h"
#include "gl_includes.h"

#include <cstddef>

namespace opengl {
namespace gl {

class UniformBuffer final
{
public:
   UniformBuffer(
      const GLenum usage );
   ~UniformBuffer( ) noexcept;

   UniformBuffer(
      UniformBuffer && ) noexcept = delete;
   UniformBuffer(
      const UniformBuffer & ) noexcept = delete;

   UniformBuffer & operator = (
      UniformBuffer && ) noexcept = delete;
   UniformBuffer & operator = (
      const UniformBuffer & ) noexcept = delete;

   GLuint GetID( ) const noexcept;
   GLenum GetTarget( ) const noexcept;

   bool SetData(
      const void * const data,
      const size_t data_size ) noexcept;
   bool SetData(
      const void * const data,
      const size_t data_offset,
      const size_t data_size ) noexcept;

   size_t GetSize( ) const noexcept;

   void Activate(
      const GLuint block_index ) noexcept;
   void Deactivate( ) noexcept;

private:
   GLuint uniform_buffer_;

   GLuint active_block_index_;

   size_t size_;
   size_t max_size_;
   
   const GLenum usage_;
   
   void (CALL_CONV * const glGenBuffers) ( GLsizei, GLuint * );
   void (CALL_CONV * const glDeleteBuffers) ( GLsizei, const GLuint * );
   void (CALL_CONV * const glBindBuffer) ( GLenum, GLuint );
   void (CALL_CONV * const glNamedBufferData) ( GLuint, std::ptrdiff_t, const void *, GLenum );
   void (CALL_CONV * const glNamedBufferSubData) ( GLuint, std::ptrdiff_t, std::ptrdiff_t, const void * );
   void (CALL_CONV * const glBindBufferBase) ( GLenum, GLuint, GLuint );

};

}} // namespace opengl::gl

#endif // _OPENGL_GL_UNIFORM_BUFFER_H_
