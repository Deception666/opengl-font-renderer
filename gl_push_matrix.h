#ifndef _OPENGL_GL_PUSH_MATRIX_H_
#define _OPENGL_GL_PUSH_MATRIX_H_

#include "gl_includes.h"

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
