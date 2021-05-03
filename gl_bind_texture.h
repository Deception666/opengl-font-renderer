#ifndef _OPENGL_GL_BIND_TEXTURE_H_
#define _OPENGL_GL_BIND_TEXTURE_H_

#if _WIN32
#include <windows.h>
#else
#error "Define for this platform!"
#endif // _WIN32

#include <gl/gl.h>

namespace opengl {
namespace gl {

class BindTexutre final
{
public:
   BindTexutre(
      const GLenum target,
      const GLuint texture_id ) noexcept;
   ~BindTexutre( ) noexcept;

private:
   const GLenum target_;
   const GLuint texture_id_;
   
};

}} // namespace opengl

#endif // _OPENGL_GL_BIND_TEXTURE_H_
