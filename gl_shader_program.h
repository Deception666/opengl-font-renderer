#ifndef _OPENGL_GL_SHADER_PROGRAM_H_
#define _OPENGL_GL_SHADER_PROGRAM_H_

#include "calling_convention.h"
#include "gl_includes.h"

#include <string>

namespace opengl {
namespace gl {

class Shader;

class ShaderProgram final
{
public:
   ShaderProgram( );
   ~ShaderProgram( ) noexcept;

   ShaderProgram(
      ShaderProgram && ) noexcept = delete;
   ShaderProgram(
      const ShaderProgram & ) noexcept = delete;

   ShaderProgram & operator = (
      ShaderProgram && ) noexcept = delete;
   ShaderProgram & operator = (
      const ShaderProgram & ) noexcept = delete;

   GLuint GetID( ) const noexcept;

   void AttachShader(
      Shader & shader ) noexcept;

   std::string LinkProgram( ) noexcept;

   void Activate( ) noexcept;
   void Deactivate( ) noexcept;

private:
   std::string GetLinkStatus( ) const noexcept;

   GLuint program_;

   GLuint (CALL_CONV * const glCreateProgram) ( );
   void (CALL_CONV * const glAttachShader) ( GLuint, GLuint );
   void (CALL_CONV * const glLinkProgram) ( GLuint );
   void (CALL_CONV * const glUseProgram) ( GLuint );
   void (CALL_CONV * const glDeleteProgram) ( GLuint );
   void (CALL_CONV * const glGetProgramiv) ( GLuint, GLenum, GLint * );
   void (CALL_CONV * const glGetProgramInfoLog) ( GLuint, GLsizei, GLsizei *, char * );

};

}} // namespace opengl::gl

#endif // _OPENGL_GL_SHADER_PROGRAM_H_
