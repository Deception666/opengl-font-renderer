#ifndef _OPENGL_GL_VALIDATE_H_
#define _OPENGL_GL_VALIDATE_H_

#if _WIN32
#include <windows.h>
#else
#error "Define for this platform!"
#endif // _WIN32

#include <gl/gl.h>

#include <cassert>

#if !NDEBUG
#if _WIN32
#define VALIDATE_NO_GL_ERROR( ) \
   assert(glGetError() == GL_NO_ERROR)
#define VALIDATE_ACTIVE_GL_CONTEXT( ) \
   assert(wglGetCurrentContext()); \
   VALIDATE_NO_GL_ERROR( )
#else
#error "Define for this platform!"
#endif // _WIN32
#else
#define VALIDATE_NO_GL_ERROR( )
#define VALIDATE_ACTIVE_GL_CONTEXT( )
#endif // !NDEBUG

#endif // _OPENGL_GL_VALIDATE_H_
