#include "gl_extensions.h"
#include "gl_defines.h"
#include "gl_get_proc_address.h"
#include "gl_validate.h"

#include <cstring>

namespace opengl {
namespace gl {

bool ExtensionIsSupported(
   const char * const extension ) noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   bool supported { false };

#ifdef _WIN32
   const auto glGetStringi =
      GetProcAddress(
         "glGetStringi",
         static_cast< const GLubyte * (*) ( GLenum, GLuint ) >(
            nullptr));
#else
#error "Define for this platform!"
#endif

   if (glGetStringi && extension)
   {
      GLint extension_count { };

      glGetIntegerv(
         GL_NUM_EXTENSIONS,
         &extension_count);

      for (GLint i { }; extension_count > i && !supported; ++i)
      {
         const auto gl_extension =
            glGetStringi(
               GL_EXTENSIONS,
               i);

         if (gl_extension)
         {
            supported =
               std::strcmp(
                  extension,
                  reinterpret_cast< const char * >(
                     gl_extension)) == 0;
         }
      }
   }

   VALIDATE_NO_GL_ERROR();

   return supported;
}

}} // namespace opengl::gl
