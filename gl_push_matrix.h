#ifndef _OPENGL_GL_PUSH_MATRIX_H_
#define _OPENGL_GL_PUSH_MATRIX_H_

#if _WIN32
#include <windows.h>
#else
#error "Define for this platform!"
#endif // _WIN32

#include <gl/gl.h>

namespace opengl {
namespace gl {

class PushMatrix final
{
public:
   PushMatrix(
      const GLenum mode ) noexcept;
   ~PushMatrix( ) noexcept;

private:
   const GLenum mode_;

};

}} // namespace opengl::gl

#endif // _OPENGL_GL_ENABLE_STATE_H_
