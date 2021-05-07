#include "gl_bind_shader_program.h"
#include "gl_shader_program.h"

namespace opengl {
namespace gl {

BindShaderProgram::BindShaderProgram(
   ShaderProgram * const shader_program ) noexcept :
shader_program_ { shader_program }
{
   if (shader_program_)
   {
      shader_program_->Activate();
   }
}

BindShaderProgram::~BindShaderProgram( ) noexcept
{
   if (shader_program_)
   {
      shader_program_->Deactivate();
   }
}

}} // namespace opengl::gl
