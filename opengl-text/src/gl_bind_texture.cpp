#include "gl_bind_texture.h"
#include "error_reporting_private.h"
#include "calling_convention.h"
#include "gl_defines.h"
#include "gl_get_proc_address.h"
#include "gl_validate.h"

#include <mutex>

namespace opengl {
namespace gl {

using glActiveTextureFuncPtr =
   void (CALL_CONV *) ( GLenum );

glActiveTextureFuncPtr GetGLActiveTexture( ) noexcept
{
   static glActiveTextureFuncPtr glActiveTexture {
      nullptr
   };

   static std::once_flag
      get_glActiveTexture;

   std::call_once(
      get_glActiveTexture,
      [ ] ( )
      {
#if _WIN32
         glActiveTexture =
            GetProcAddress(
               "glActiveTexture",
               glActiveTexture);
#else
#error "Define for this platform!"
#endif

         if (!glActiveTexture)
         {
            ReportError(
               "Unable to obtain function pointer "
               "for glActiveTexture.  Texture "
               "operations may be compromised.");
         }
      });

   return glActiveTexture;
}

BindTexutre::BindTexutre(
   const GLenum target,
   const GLuint texture_id,
   const GLenum texture_unit ) noexcept :
target_ { target },
texture_id_ { texture_id },
texture_unit_ { texture_unit }
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   const auto glActiveTexture =
      GetGLActiveTexture();

   if (glActiveTexture)
   {
      glActiveTexture(
         texture_unit_);
   }

   glBindTexture(
      target_,
      texture_id_);

   VALIDATE_NO_GL_ERROR();
}

BindTexutre::~BindTexutre() noexcept
{
   VALIDATE_ACTIVE_GL_CONTEXT();

   const auto glActiveTexture =
      GetGLActiveTexture();

   if (glActiveTexture)
   {
      glActiveTexture(
         texture_unit_);
   }

   glBindTexture(
      target_,
      0);

   if (glActiveTexture)
   {
      glActiveTexture(
         GL_TEXTURE0_ARB);
   }

   VALIDATE_NO_GL_ERROR();
}

}} // namespace opengl
