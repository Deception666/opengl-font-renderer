#ifndef _OPENGL_GL_ENABLE_STATE_H_
#define _OPENGL_GL_ENABLE_STATE_H_

#include "gl_includes.h"

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
