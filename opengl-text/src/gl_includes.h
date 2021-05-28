#ifndef _OPENGL_GL_INCLUDES_H_
#define _OPENGL_GL_INCLUDES_H_

#if _WIN32
#include <windows.h>
#elif __linux__
#include <GL/glx.h>
#else
#error "Define for this platform!"
#endif // _WIN32

#include <GL/gl.h>

#endif // _OPENGL_GL_INCLUDES_H_
