#include "gl_uniform_buffer.h"
#include "gl_defines.h"
#include "gl_get_proc_address.h"
#include "gl_validate.h"

#include <algorithm>
#include <stdexcept>

namespace opengl {
namespace gl {

UniformBuffer::UniformBuffer(
   const GLenum usage ) :
uniform_buffer_ { },
active_block_index_ { GL_INVALID_INDEX },
size_ { },
max_size_ { },
usage_ { usage },
glGenBuffers { GetProcAddress("glGenBuffers", glGenBuffers) },
glDeleteBuffers { GetProcAddress("glDeleteBuffers", glDeleteBuffers) },
glBindBuffer { GetProcAddress("glBindBuffer", glBindBuffer) },
glNamedBufferData { GetProcAddress("glNamedBufferData", glNamedBufferData) },
glNamedBufferSubData { GetProcAddress("glNamedBufferSubData", glNamedBufferSubData) },
glBindBufferBase { GetProcAddress("glBindBufferBase", glBindBufferBase) }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!glGenBuffers || !glDeleteBuffers ||
       !glBindBuffer || !glNamedBufferData ||
       !glNamedBufferSubData || !glBindBufferBase)
   {
      throw
         std::runtime_error(
            "Not all gl uniform buffer entry points defined!");
   }

   glGenBuffers(
      1,
      &uniform_buffer_);

   if (!uniform_buffer_)
   {
      throw
         std::runtime_error(
            "Unable to create a uniform buffer object!");
   }

   glNamedBufferData(
      uniform_buffer_,
      0,
      nullptr,
      usage_);

   VALIDATE_NO_GL_ERROR();
}

UniformBuffer::~UniformBuffer( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glDeleteBuffers(
      1,
      &uniform_buffer_);

   VALIDATE_NO_GL_ERROR();
}

GLuint UniformBuffer::GetID( ) const noexcept
{
   return
      uniform_buffer_;
}

GLenum UniformBuffer::GetTarget( ) const noexcept
{
   return
      GL_UNIFORM_BUFFER;
}

bool UniformBuffer::SetData(
   const void * const data,
   const size_t data_size ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   bool set { false };

   if (data_size > max_size_)
   {
      glNamedBufferData(
         uniform_buffer_,
         data_size,
         data,
         usage_);
   }
   else if (data)
   {
      glNamedBufferSubData(
         uniform_buffer_,
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

size_t UniformBuffer::GetSize( ) const noexcept
{
   return
      size_;
}

void UniformBuffer::Activate(
   const GLuint block_index ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   Deactivate();

   if (GL_INVALID_INDEX != block_index)
   {
      glBindBufferBase(
         GL_UNIFORM_BUFFER,
         block_index,
         uniform_buffer_);

      active_block_index_ =
         block_index;
   }

   VALIDATE_NO_GL_ERROR();
}

void UniformBuffer::Deactivate( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (GL_INVALID_INDEX != active_block_index_)
   {
      glBindBufferBase(
         GL_UNIFORM_BUFFER,
         active_block_index_,
         0);

      active_block_index_ =
         GL_INVALID_INDEX;
   }

   VALIDATE_NO_GL_ERROR();
}

}} // namespace opengl::gl
