#ifndef _OPENGL_GL_ENABLE_STATE_H_
#define _OPENGL_GL_ENABLE_STATE_H_

#if _WIN32
#include <windows.h>
#else
#error "Define for this platform!"
#endif // _WIN32

#include <gl/gl.h>

namespace opengl {
namespace gl {

class EnableState final
{
public:
   EnableState(
      const GLenum state ) noexcept;
   ~EnableState( ) noexcept;

private:
   const GLenum state_;

};

}} // namespace opengl::gl

#endif // _OPENGL_GL_ENABLE_STATE_H_
