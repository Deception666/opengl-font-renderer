#ifndef _OPENGL_LIBRARY_EXPORTS_H_
#define _OPENGL_LIBRARY_EXPORTS_H_

#if _WIN32

   #if OPENGL_TEXT_DLL
      #ifdef opengl_text_EXPORTS
         #define OGL_TEXT_EXPORT __declspec( dllexport )
      #else
         #define OGL_TEXT_EXPORT __declspec( dllimport )
      #endif
   #else
      #define OGL_TEXT_EXPORT
   #endif

#else

   #define OGL_TEXT_EXPORT

#endif

#endif // _OPENGL_LIBRARY_EXPORTS_H_
