#include "gl_vertex_array.h"
#include "gl_get_proc_address.h"
#include "gl_validate.h"
#include "gl_vertex_buffer.h"

#include <stdexcept>

namespace opengl {
namespace gl {

VertexArray::VertexArray( ) :
vertex_array_ { },
glCreateVertexArrays { GetProcAddress("glCreateVertexArrays", glCreateVertexArrays) },
glBindVertexArray { GetProcAddress("glBindVertexArray", glBindVertexArray) },
glDeleteVertexArrays { GetProcAddress("glDeleteVertexArrays", glDeleteVertexArrays) },
glVertexArrayVertexBuffer { GetProcAddress("glVertexArrayVertexBuffer", glVertexArrayVertexBuffer) },
glEnableVertexArrayAttrib { GetProcAddress("glEnableVertexArrayAttrib", glEnableVertexArrayAttrib) },
glDisableVertexArrayAttrib { GetProcAddress("glDisableVertexArrayAttrib", glDisableVertexArrayAttrib) },
glVertexArrayAttribFormat { GetProcAddress("glVertexArrayAttribFormat", glVertexArrayAttribFormat) },
glVertexArrayAttribBinding { GetProcAddress("glVertexArrayAttribBinding", glVertexArrayAttribBinding) }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!glCreateVertexArrays || !glBindVertexArray ||
       !glDeleteVertexArrays || !glVertexArrayVertexBuffer ||
       !glEnableVertexArrayAttrib || !glDisableVertexArrayAttrib ||
       !glVertexArrayAttribFormat || !glVertexArrayAttribBinding)
   {
      throw
         std::runtime_error(
            "Not all gl buffer entry points defined!");
   }

   glCreateVertexArrays(
      1,
      &vertex_array_);

   if (!vertex_array_)
   {
      throw
         std::runtime_error(
            "Unable to create a buffer object!");
   }

   VALIDATE_NO_GL_ERROR();
}

VertexArray::~VertexArray( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glDeleteVertexArrays(
      1,
      &vertex_array_);

   VALIDATE_NO_GL_ERROR();
}

GLuint VertexArray::GetID( ) const noexcept
{
   return
      vertex_array_;
}

void VertexArray::BindVertexBuffer(
   const VertexBuffer & buffer,
   const GLuint binding_index,
   const std::ptrdiff_t base_offset,
   const GLsizei element_stride ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glVertexArrayVertexBuffer(
      vertex_array_,
      binding_index,
      buffer.GetID(),
      base_offset,
      element_stride);

   VALIDATE_NO_GL_ERROR();
}

void VertexArray::BindVertexAttribute(
   const GLuint binding_index,
   const GLuint attribute_index,
   const GLint component_count,
   const GLenum component_type,
   const GLboolean normalize_data,
   const GLuint relative_offset ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glVertexArrayAttribFormat(
      vertex_array_,
      attribute_index,
      component_count,
      component_type,
      normalize_data,
      relative_offset);
   
   glVertexArrayAttribBinding(
      vertex_array_,
      attribute_index,
      binding_index);

   VALIDATE_NO_GL_ERROR();
}

void VertexArray::EnableVertexAttribute(
   const GLuint attribute_index ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();
   
   glEnableVertexArrayAttrib(
      vertex_array_,
      attribute_index);

   VALIDATE_NO_GL_ERROR();
}

void VertexArray::DisableVertexAttribute(
   const GLuint attribute_index ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glDisableVertexArrayAttrib(
      vertex_array_,
      attribute_index);

   VALIDATE_NO_GL_ERROR();
}

void VertexArray::Activate( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glBindVertexArray(
      vertex_array_);

   VALIDATE_NO_GL_ERROR();
}

void VertexArray::Deactivate( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glBindVertexArray(
      0);

   VALIDATE_NO_GL_ERROR();
}

}} // namespace opengl::gl
