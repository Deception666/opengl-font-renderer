#ifndef _OPENGL_GL_SHADER_H_
#define _OPENGL_GL_SHADER_H_

#include "calling_convention.h"
#include "gl_includes.h"

#include <string>

namespace opengl {
namespace gl {

class Shader final
{
public:
   Shader(
      const GLenum shader_type );
   ~Shader( ) noexcept;

   Shader(
      Shader && ) noexcept = delete;
   Shader(
      const Shader & ) noexcept = delete;

   Shader & operator = (
      Shader && ) noexcept = delete;
   Shader & operator = (
      const Shader & ) noexcept = delete;

   GLuint GetID( ) const noexcept;

   std::string SetSource(
      const std::string & source ) noexcept;
   std::string SetSource(
      const char * const source ) noexcept;

private:
   std::string GetCompileStatus( ) const noexcept;

   GLuint shader_;
   const GLenum shader_type_;

   GLuint (CALL_CONV * const glCreateShader) ( GLenum );
   void (CALL_CONV * const glDeleteShader) ( GLuint );
   void (CALL_CONV * const glShaderSource) ( GLuint, GLsizei, const char **, const GLint * );
   void (CALL_CONV * const glCompileShader) ( GLuint );
   void (CALL_CONV * const glGetShaderInfoLog) ( GLuint, GLsizei, GLsizei *, char * );
   void (CALL_CONV * const glGetShaderiv) ( GLuint, GLenum, GLint * );

};

}} // namespace opengl::gl

#endif // _OPENGL_GL_SHADER_H_
