#include "gl_shader.h"
#include "gl_defines.h"
#include "gl_get_proc_address.h"
#include "gl_validate.h"

#include <stdexcept>

namespace opengl {
namespace gl {

Shader::Shader(
   const GLenum shader_type ) :
shader_ { 0 },
shader_type_ { shader_type },
glCreateShader { GetProcAddress("glCreateShader", glCreateShader) },
glDeleteShader { GetProcAddress("glDeleteShader", glDeleteShader) },
glShaderSource { GetProcAddress("glShaderSource", glShaderSource) },
glCompileShader { GetProcAddress("glCompileShader", glCompileShader) },
glGetShaderInfoLog { GetProcAddress("glGetShaderInfoLog", glGetShaderInfoLog) },
glGetShaderiv { GetProcAddress("glGetShaderiv", glGetShaderiv) }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!glCreateShader || !glDeleteShader ||
       !glShaderSource || !glCompileShader ||
       !glGetShaderInfoLog || !glGetShaderiv)
   {
      throw
         std::runtime_error(
            "Not all gl shader entry points defined!");
   }

   shader_ =
      glCreateShader(
         shader_type_);

   if (!shader_)
   {
      throw
         std::runtime_error(
            "Unable to create a shader object!");
   }

   VALIDATE_NO_GL_ERROR();
}

Shader::~Shader( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (shader_)
   {
      glDeleteShader(
         shader_);
   }

   VALIDATE_NO_GL_ERROR();
}

GLuint Shader::GetID( ) const noexcept
{
   return
      shader_;
}

std::string Shader::SetSource(
   const std::string & source ) noexcept
{
   return
      SetSource(
         source.c_str());
}

std::string Shader::SetSource(
   const char * const source ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   std::string error;

   if (!source)
   {
      error =
         "A valid source pointer is required!";
   }
   else
   {
      glShaderSource(
         shader_,
         1,
         const_cast< const char ** >(
            &source),
         nullptr);

      glCompileShader(
         shader_);

      error =
         GetCompileStatus();
   }

   VALIDATE_NO_GL_ERROR();

   return error;
}

std::string Shader::GetCompileStatus( ) const noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   std::string error;

   GLint compiled_status {
      GL_FALSE };

   glGetShaderiv(
      shader_,
      GL_COMPILE_STATUS,
      &compiled_status);

   if (GL_FALSE == compiled_status)
   {
      GLint log_length { };

      glGetShaderiv(
         shader_,
         GL_INFO_LOG_LENGTH,
         &log_length);

      error.assign(
         log_length - 1,
         ' ');

      GLsizei length { };

      glGetShaderInfoLog(
         shader_,
         log_length,
         &length,
         error.data());
   }

   VALIDATE_NO_GL_ERROR();

   return error;
}

}} // namespace opengl::gl
