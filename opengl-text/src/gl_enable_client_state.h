#ifndef _OPENGL_GL_ENABLE_CLIENT_STATE_H_
#define _OPENGL_GL_ENABLE_CLIENT_STATE_H_

#include "gl_includes.h"

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

}} // namespace opengl::gl

#endif // _OPENGL_GL_ENABLE_CLIENT_STATE_H_
