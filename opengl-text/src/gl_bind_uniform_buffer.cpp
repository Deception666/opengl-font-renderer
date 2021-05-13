#include "gl_bind_uniform_buffer.h"
#include "gl_defines.h"
#include "gl_includes.h"
#include "gl_shader_program.h"
#include "gl_uniform_buffer.h"

namespace opengl {
namespace gl {

BindUniformBuffer::BindUniformBuffer(
   UniformBuffer * const uniform_buffer,
   const ShaderProgram * const shader_program,
   const char * const uniform_block_name  ) noexcept :
uniform_buffer_ { uniform_buffer }
{
   if (uniform_buffer_ &&
       shader_program &&
       uniform_block_name)
   {
      const auto block_index =
         shader_program->GetUniformBlockIndex(
            uniform_block_name);

      if (GL_INVALID_INDEX != block_index)
      {
         uniform_buffer_->Activate(
            block_index);
      }
   }
}

BindUniformBuffer::~BindUniformBuffer( ) noexcept
{
   if (uniform_buffer_)
   {
      uniform_buffer_->Deactivate();
   }
}

}} // namespace opengl::gl
