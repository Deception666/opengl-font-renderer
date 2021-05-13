#ifndef _OPENGL_GL_GET_PROC_ADDRESS_H_
#define _OPENGL_GL_GET_PROC_ADDRESS_H_

#include "gl_includes.h"
#include "gl_validate.h"

namespace opengl {
namespace gl {

template < typename T >
T GetProcAddress(
   const char * const function,
   T ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

#if _WIN32

   return
      reinterpret_cast< T >(
         wglGetProcAddress(function));

#else
#error "Define for this platform!"
#endif

}

}} // namespace opengl::gl

#endif // _OPENGL_GL_GET_PROC_ADDRESS_H_
