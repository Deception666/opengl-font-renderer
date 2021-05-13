#ifndef _OPENGL_GL_VERTEX_ARRAY_H_
#define _OPENGL_GL_VERTEX_ARRAY_H_

#include "calling_convention.h"
#include "gl_includes.h"

#include <cstddef>

namespace opengl {
namespace gl {

class VertexBuffer;

class VertexArray final
{
public:
   VertexArray( );
   ~VertexArray( ) noexcept;

   VertexArray(
      VertexArray && ) noexcept = delete;
   VertexArray(
      const VertexArray & ) noexcept = delete;

   VertexArray & operator = (
      VertexArray && ) noexcept = delete;
   VertexArray & operator = (
      const VertexArray & ) noexcept = delete;

   GLuint GetID( ) const noexcept;

   void BindVertexBuffer(
      const VertexBuffer & buffer,
      const GLuint binding_index,
      const std::ptrdiff_t base_offset,
      const GLsizei element_stride ) noexcept;

   void BindVertexAttribute(
      const GLuint binding_index,
      const GLuint attribute_index,
      const GLint component_count,
      const GLenum component_type,
      const GLboolean normalize_data,
      const GLuint relative_offset ) noexcept;

   void EnableVertexAttribute(
      const GLuint attribute_index ) noexcept;
   void DisableVertexAttribute(
      const GLuint attribute_index ) noexcept;

   void Activate( ) noexcept;
   void Deactivate( ) noexcept;

private:
   GLuint vertex_array_;
   
   void (CALL_CONV * const glGenVertexArrays) ( GLsizei, GLuint * );
   void (CALL_CONV * const glBindVertexArray) ( GLuint );
   void (CALL_CONV * const glDeleteVertexArrays) ( GLsizei, const GLuint * );
   void (CALL_CONV * const glVertexArrayVertexBuffer) ( GLuint, GLuint, GLuint, std::ptrdiff_t, GLsizei );
   void (CALL_CONV * const glEnableVertexArrayAttrib) ( GLuint, GLuint );
   void (CALL_CONV * const glDisableVertexArrayAttrib) ( GLuint, GLuint );
   void (CALL_CONV * const glVertexArrayAttribFormat) ( GLuint, GLuint, GLint, GLenum, GLboolean, GLuint );
   void (CALL_CONV * const glVertexArrayAttribBinding) ( GLuint, GLuint, GLuint );

};

}} // namespace opengl::gl

#endif // _OPENGL_GL_VERTEX_ARRAY_H_
