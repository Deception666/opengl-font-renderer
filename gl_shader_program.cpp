#include "gl_shader_program.h"
#include "gl_defines.h"
#include "gl_get_proc_address.h"
#include "gl_shader.h"
#include "gl_validate.h"

#include <stdexcept>

namespace opengl {
namespace gl {

ShaderProgram::ShaderProgram( ) :
program_ { 0 },
glCreateProgram { GetProcAddress("glCreateProgram", glCreateProgram) },
glAttachShader { GetProcAddress("glAttachShader", glAttachShader) },
glLinkProgram { GetProcAddress("glLinkProgram", glLinkProgram) },
glUseProgram { GetProcAddress("glUseProgram", glUseProgram) },
glDeleteProgram { GetProcAddress("glDeleteProgram", glDeleteProgram) },
glGetProgramiv { GetProcAddress("glGetProgramiv", glGetProgramiv) },
glGetProgramInfoLog { GetProcAddress("glGetProgramInfoLog", glGetProgramInfoLog) },
glGetUniformBlockIndex { GetProcAddress("glGetUniformBlockIndex", glGetUniformBlockIndex) }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   if (!glCreateProgram || !glAttachShader ||
       !glLinkProgram || !glUseProgram ||
       !glDeleteProgram || !glGetProgramiv ||
       !glGetProgramInfoLog || !glGetUniformBlockIndex)
   {
      throw
         std::runtime_error(
            "Not all gl shader program entry points defined!");
   }

   program_ =
      glCreateProgram();

   if (!program_)
   {
      throw
         std::runtime_error(
            "Unable to create a shader program object!");
   }

   VALIDATE_NO_GL_ERROR();
}

ShaderProgram::~ShaderProgram( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glDeleteProgram(
      program_);

   VALIDATE_NO_GL_ERROR();
}

GLuint ShaderProgram::GetID( ) const noexcept
{
   return
      program_;
}

void ShaderProgram::AttachShader(
   Shader & shader ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glAttachShader(
      program_,
      shader.GetID());

   VALIDATE_NO_GL_ERROR();
}

std::string ShaderProgram::LinkProgram( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glLinkProgram(
      program_);

   std::string error =
      GetLinkStatus();

   VALIDATE_NO_GL_ERROR();

   return error;
}

GLuint ShaderProgram::GetUniformBlockIndex(
   const char * const uniform_block_name ) const noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   GLuint index {
      GL_INVALID_INDEX };

   if (uniform_block_name)
   {
      index =
         glGetUniformBlockIndex(
            program_,
            uniform_block_name);
   }

   VALIDATE_NO_GL_ERROR();

   return
      index;
}

void ShaderProgram::Activate( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glUseProgram(
      program_);

   VALIDATE_NO_GL_ERROR();
}

void ShaderProgram::Deactivate( ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   glUseProgram(
      0);

   VALIDATE_NO_GL_ERROR();
}

std::string ShaderProgram::GetLinkStatus( ) const noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   std::string error;

   GLint link_status {
      GL_FALSE };

   glGetProgramiv(
      program_,
      GL_LINK_STATUS,
      &link_status);

   if (GL_FALSE == link_status)
   {
      GLint log_length { };

      glGetProgramiv(
         program_,
         GL_INFO_LOG_LENGTH,
         &log_length);

      error.assign(
         log_length,
         ' ');

      GLsizei length { };

      glGetProgramInfoLog(
         program_,
         log_length,
         &length,
         error.data());
   }

   VALIDATE_NO_GL_ERROR();

   return error;
}

}} // namespace opengl::gl
