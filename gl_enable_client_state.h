#ifndef _OPENGL_GL_ENABLE_CLIENT_STATE_H_
#define _OPENGL_GL_ENABLE_CLIENT_STATE_H_

#if _WIN32
#include <windows.h>
#else
#error "Define for this platform!"
#endif // _WIN32

#include <gl/gl.h>

namespace opengl {
namespace gl {

class EnableClientState final
{
public:
   EnableClientState(
      const GLenum state ) noexcept;
   ~EnableClientState( ) noexcept;

private:
   const GLenum state_;

};

}} // namespace opengl

#endif // _OPENGL_GL_ENABLE_CLIENT_STATE_H_
