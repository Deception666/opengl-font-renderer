#ifndef _OPENGL_GL_BIND_TEXTURE_H_
#define _OPENGL_GL_BIND_TEXTURE_H_

#include "gl_includes.h"

namespace opengl {
namespace gl {

class BindTexutre final
{
public:
   BindTexutre(
      const GLenum target,
      const GLuint texture_id,
      const GLenum texture_unit ) noexcept;
   ~BindTexutre( ) noexcept;

private:
   const GLenum target_;
   const GLuint texture_id_;
   const GLenum texture_unit_;
   
};

}} // namespace opengl::gl

#endif // _OPENGL_GL_BIND_TEXTURE_H_
