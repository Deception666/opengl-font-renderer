#include "gl_vertex_buffer.h"
#include "gl_defines.h"
#include "gl_get_proc_address.h"
#include "gl_validate.h"

#include <algorithm>
#include <stdexcept>

namespace opengl {
namespace gl {

VertexBuffer::VertexBuffer(
   const GLenum usage ) :
vertex_buffer_ { },
size_ { },
max_size_ { },
usage_ { usage },
glGenBuffers { GetProcAddress("glGenBuffers", glGenBuffers) },
glDeleteBuffers { GetProcAddress("glDeleteBuffers", glDeleteBuffers) },
glBindBuffer { GetProcAddress("glBindBuffer", glBindBuffer) },
glNamedBufferData { GetProcAddress("glNamedBufferData", glNamedBufferData) },
glNamedBufferSubData { GetProcAddress("glNamedBufferSubData", glNamedBufferSubData) }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!glGenBuffers || !glDeleteBuffers ||
       !glBindBuffer || !glNamedBufferData ||
       !glNamedBufferSubData)
   {
      throw
         std::runtime_error(
            "Not all gl buffer entry points defined!");
   }

   glGenBuffers(
      1,
      &vertex_buffer_);

   if (!vertex_buffer_)
   {
      throw
         std::runtime_error(
            "Unable to create a buffer object!");
   }

   glNamedBufferData(
      vertex_buffer_,
      0,
      nullptr,
      usage_);

   VALIDATE_NO_GL_ERROR();
}

VertexBuffer::~VertexBuffer( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glDeleteBuffers(
      1,
      &vertex_buffer_);

   VALIDATE_NO_GL_ERROR();
}

GLuint VertexBuffer::GetID( ) const noexcept
{
   return
      vertex_buffer_;
}

GLenum VertexBuffer::GetTarget( ) const noexcept
{
   return
      GL_ARRAY_BUFFER_ARB;
}

bool VertexBuffer::SetData(
   const void * const data,
   const size_t data_size ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   bool set { false };

   if (data_size > max_size_)
   {
      glNamedBufferData(
         vertex_buffer_,
         data_size,
         data,
         usage_);
   }
   else if (data)
   {
      glNamedBufferSubData(
         vertex_buffer_,
         0,
         data_size,
         data);
   }

   size_ = data_size;
   
   max_size_ =
      std::max(max_size_, size_);

   set = true;

   VALIDATE_NO_GL_ERROR();

   return set;
}

size_t VertexBuffer::GetSize( ) const noexcept
{
   return
      size_;
}

}} // namespace opengl::gl
