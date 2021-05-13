#ifndef _OPENGL_CALLING_CONVENTION_H_
#define _OPENGL_CALLING_CONVENTION_H_

#if _WIN32

#if _M_IX86
#define CALL_CONV __stdcall
#elif _M_X64
#define CALL_CONV
#else
#define "Define for this platform type!"
#endif // _M_IX86

#else
#error "Define for this platform!"
#endif // _WIN32

#endif // _OPENGL_CALLING_CONVENTION_H_
